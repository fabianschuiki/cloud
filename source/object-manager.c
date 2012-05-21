/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object-manager.h"
#include "object.h"
#include "connection.h"


struct cld_object_pool_entry {
	int id;
	struct cld_object *object;
};

struct cld_object_pool {
	struct cld_object_pool_entry *entries;
	int count;
	int size;
};

static struct cld_object_pool *
cld_object_pool_create ()
{
	struct cld_object_pool *pool;
	
	pool = malloc(sizeof *pool);
	if (pool == NULL)
		return NULL;
	
	pool->count = 0;
	pool->size = 1;
	pool->entries = malloc(sizeof *pool->entries);
	if (pool->entries == NULL) {
		free(pool);
		return NULL;
	}
	
	return pool;
}

static void
cld_object_pool_destroy (struct cld_object_pool *pool)
{
	free(pool->entries);
	free(pool);
}

/** Resizes the pool to be able to hold size number of entries. */
static void
cld_object_pool_resize (struct cld_object_pool *pool, int size)
{
	pool->entries = realloc(pool->entries, sizeof(*pool->entries) * size);
	assert(pool->entries && "unable to realloc pool entries");
}

/** Ensures that another count objects fit into the pool. */
static void
cld_object_pool_extend (struct cld_object_pool *pool, int count)
{
	int nc = pool->count + count;
	int size = pool->size;
	while (nc > size) size <<= 1;
	if (size != pool->size)
		cld_object_pool_resize(pool, size);
}

/** Returns the location of the given ID in the entries of the pool. Note that
 * there might be no entry with the given ID. */
static int
cld_object_pool_locate (struct cld_object_pool *pool, int id)
{
	int index = 0;
	int a = 0;
	int b = pool->count;
	while (a != b) {
		index = (a+b)/2;
		int subject = pool->entries[index].id;
		if (subject == id)
			return index;
		if (subject < id)
			a = index;
		if (subject > id)
			b = index + 1;
	}
	return index;
}

/** Returns the index of the entry with the given ID, or -1 if none was found */
static int
cld_object_pool_find (struct cld_object_pool *pool, int id)
{
	int i = cld_object_pool_locate(pool, id);
	if (i >= pool->count || pool->entries[i].id != id)
		return -1;
	return i;
}

/** Adds the given object to the pool. */
static void
cld_object_pool_add (struct cld_object_pool *pool, struct cld_object *object)
{
	int index = cld_object_pool_locate(pool, object->id);
	cld_object_pool_extend(pool, 1);
	
	memcpy(&pool->entries[index + 1], &pool->entries[index], pool->count - index);
	pool->entries[index].id = object->id;
	pool->entries[index].object = object;
}

/** Removes the given object from the pool. Does nothing if the object is not
 * in the pool. */
static void
cld_object_pool_remove (struct cld_object_pool *pool, int id)
{
	int index = cld_object_pool_find(pool, id);
	if (index < 0)
		return;
	
	memcpy(&pool->entries[index], &pool->entries[index + 1], pool->count - index - 1);
	pool->count--;
}

/** Returns the object with the given id, or NULL if it is not in the pool. */
static struct cld_object *
cld_object_pool_get (struct cld_object_pool *pool, int id)
{
	int index = cld_object_pool_find(pool, id);
	if (index < 0)
		return NULL;
	return pool->entries[index].object;
}



struct cld_object_manager {
	struct cld_connection *connection;
	int current_id;
	int max_id;
	
	struct cld_object_pool *objects;
	struct cld_object_pool *local;
	
	cld_object_manager_send_func_t send;
	void *send_data;
};


struct cld_object_manager *
cld_object_manager_create ()
{
	struct cld_object_manager *manager;
	
	manager = malloc(sizeof *manager);
	if (manager == NULL)
		return NULL;
	
	memset(manager, 0, sizeof *manager);
	
	manager->objects = cld_object_pool_create();
	if (manager->objects == NULL) {
		free(manager);
		return NULL;
	}
	
	manager->local = cld_object_pool_create();
	if (manager->local == NULL) {
		cld_object_pool_destroy(manager->objects);
		free(manager);
		return NULL;
	}
	
	return manager;
}

void
cld_object_manager_destroy (struct cld_object_manager *manager)
{
	cld_object_pool_destroy(manager->objects);
	cld_object_pool_destroy(manager->local);
	free(manager);
}


/** Add an object created locally to the object manager. This will cause the
 * object to be shared with other clients, daemons and services. */
void
cld_object_manager_add (struct cld_object_manager *manager, struct cld_object *object)
{
	if (manager->current_id >= manager->max_id) {
		//TODO: request new id range, sending a CLD_REQUEST_ID_RANGE.
	}
	object->id = manager->current_id++;
	
	cld_object_pool_add(manager->local, object);
	cld_object_pool_add(manager->objects, object);
	printf("added %x\n", object->id);
	
	//Advertize the object.
	struct cld_message msg;
	msg.op = CLD_MSG_ADVERTISE;
	msg.connection = NULL;
	msg.obj.id = object->id;
	msg.obj.type = object->type;
	cld_object_manager_send(manager, &msg);
}

/** Removes an object created locally from the object manager. */
void
cld_object_manager_remove (struct cld_object_manager *manager, int id)
{
	//Publish object deletion.
	struct cld_message msg;
	msg.op = CLD_MSG_DESTROYED;
	msg.connection = NULL;
	msg.obj.id = id;
	cld_object_manager_send(manager, &msg);
	
	cld_object_pool_remove(manager->local, id);
	cld_object_pool_remove(manager->objects, id);
	printf("removed %x\n", id);
}


/** Handles incoming messages such as object creation/destruction advertisements,
 * object updates, requests, id ranges, etc. */
int
cld_object_manager_handle (struct cld_object_manager *manager, struct cld_message *msg)
{
	switch (msg->op) {
		case CLD_MSG_ADVERTISE: {
			printf("remote %x created (a %s)\n", msg->obj.id, msg->obj.type);
			//TODO: inform observers about the new object, potentially updating smart lists.
		} break;
		
		case CLD_MSG_DESTROYED: {
			printf("remote %x destroyed\n", msg->obj.id);
			//TODO: remove object from the pool, inform observers that the object was deleted.
		} break;
		
		case CLD_MSG_REQUEST_OBJECT: {
			struct cld_object *object = cld_object_pool_get(manager->local, msg->obj.id);
			if (object == NULL) {
				//TODO: return error that the object wasn't found
			} else {
				//TODO: return the object.
				struct cld_message resp;
				resp.op = CLD_MSG_OBJECT;
				resp.connection = msg->connection;
				resp.obj.id = object->id;
				resp.obj.object = object;
				cld_object_manager_send(manager, &resp);
			}
		} break;
		
		case CLD_MSG_ID_RANGE: {
			manager->current_id = msg->id.min;
			manager->max_id = msg->id.max;
			printf("received new ID range [%x,%x]\n", manager->current_id, manager->max_id);
		} break;
	}
	return 0;
}

void
cld_object_manager_send_func (struct cld_object_manager *manager, cld_object_manager_send_func_t func, void *data)
{
	manager->send = func;
	manager->send_data = data;
}

void
cld_object_manager_send (struct cld_object_manager *manager, struct cld_message *message)
{
	manager->send(message, manager->send_data);
}