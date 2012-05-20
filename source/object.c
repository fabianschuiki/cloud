/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "object.h"
#include "buffer.h"
#include "object-manager.h"


/** Creates a new object of the given kind and registers it with the given
 * object manager. */
struct cld_object *
cld_object_create (struct cld_object_manager *manager, unsigned char kind)
{
	struct cld_object *object;
	
	object = malloc(sizeof *object);
	if (object == NULL)
		return NULL;
	
	memset(manager, 0, sizeof *manager);
	object->manager = manager;
	object->kind = kind;
	
	cld_object_manager_add(manager, object);
	return object;
}

/** Destroys the given object and removes it from its object manager. */
void
cld_object_destroy (struct cld_object *object)
{
	//TODO: iterate through the fields and destroy each.
	
	cld_object_manager_remove(object->manager, object->id);
	free(object);
}



struct cld_object *
cld_object_create_object (struct cld_object_manager *manager, const char *type)
{
	struct cld_object *object = cld_object_create(manager, CLD_TYPE_OBJECT);
	if (object == NULL)
		return NULL;
	
	object->kind = strdup(type);
	return object;
}

struct cld_object *
cld_object_create_string (struct cld_object_manager *manager, const char *string)
{
	struct cld_object *object = cld_object_create(manager, CLD_TYPE_STRING);
	if (object == NULL)
		return NULL;
	
	object->string = strdup(string);
	return object;
}

struct cld_object *
cld_object_create_array (struct cld_object_manager *manager)
{
	return cld_object_create(manager, CLD_TYPE_ARRAY);
}



/** Increases the object's reference count.
 * 
 * Call this function whenever you store a reference to an object. This will
 * prevent the object from being freed. Additionally, the object manager of
 * client and service objects use the reference count to determine when you're
 * no longer interested in an object, in order to stop tracking it. */
void
cld_object_ref (struct cld_object *object)
{
	object->refcount++;
}

/** Reduces the object's reference count. If the count drops to 0, the object
 * is destroyed immediately.
 * 
 * Call this function whenever you no longer need and object. Make sure you
 * don't use the object afterwards, as it might already be destroyed. */
void
cld_object_unref (struct cld_object *object)
{
	object->refcount--;
	if (object->refcount <= 0)
		cld_object_destroy(object);
}



/** Returns whether the object is of the given object type. */
int
cld_object_is_object (struct cld_object *object, const char *type)
{
	if (object->kind != CLD_TYPE_OBJECT)
		return 0;
	if (type == strcmp(object->type, type) != 0)
		return 0;
	return 1;
}

/** Returns whether the object is a string. */
int
cld_object_is_string (struct cld_object *object)
{
	return object->kind == CLD_TYPE_STRING;
}

/** Returns whether the object is an array. */
int
cld_object_is_array (struct cld_object *object)
{
	return object->kind == CLD_TYPE_ARRAY;
}



void
cld_object_fields_resize (struct cld_object *object, int max)
{
	object->max_fields = max;
	object->fields = realloc(object->fields, max * sizeof(*object->fields));
}

void
cld_object_fields_extend (struct cld_object *object, int num)
{
	int new_num = object->num_fields + num;
	
	int max = object->max_fields;
	if (max <= 0)
		max = 1;
	
	while (new_num > max) max <<= 1;
	
	if (max != object->max_fields)
		cld_object_fields_resize(object, max);
}

/** Creates a new field and returns it. */
struct cld_object_field *
cld_object_add_field (struct cld_object *object)
{
	cld_object_fields_extend(object, 1);
	object->num_fields++;
	
	struct cld_object_field *field = &object->fields[object->num_fields-1];
	field->name = NULL;
	field->object = NULL;
	return field;
}

/** Removes the field at the given index. If the field as an object set it is
 * NOT unref'd automatically! */
void
cld_object_remove_field (struct cld_object *object, int index)
{
	assert(index >= 0 && index < object->num_fields && "field index to remove out of bounds");
	memcpy(&object->fields[index], &object->fields[index + 1], object->num_fields - index - 1);
	object->num_fields--;
}

/** Returns the field with the given name, or NULL if it does not exist. */
struct cld_object_field *
cld_object_find_field (struct cld_object *object, const char *name)
{
	int i;
	for (i = 0; i < object->num_fields; i++) {
		if (strcmp(object->fields[i].name, name) == 0)
			return &object->fields[i];
	}
	return NULL;
}

/** Returns the field with the given name, creating it if required. */
struct cld_object_field *
cld_object_get_field (struct cld_object *object, const char *name)
{
	struct cld_object_field *field = cld_object_find_field(object, name);
	if (field == NULL) {
		field = cld_object_add_field(object);
		field->name = strdup(name);
	}
	return field;
}



void
cld_object_set (struct cld_object *object, const char *name, struct cld_object *value)
{
	struct cld_object_field *field = cld_object_get_field(object, name);
	if (value) cld_object_ref(value);
	if (field->object) cld_object_unref(field->object);
	field->object = value;
	//TODO: inform object manager
}

struct cld_object *
cld_object_get (struct cld_object *object, const char *name)
{
	struct cld_object_field *field = cld_object_find_field(object, name);
	if (field == NULL)
		return NULL;
	return field->object;
}


/** Adds the given object. */
int
cld_object_add (struct cld_object *object, struct cld_object *value)
{
	int index = object->num_fields;
	struct cld_object_field *field = cld_object_add_field(object);
	cld_object_ref(value);
	field->object = value;
	//TODO: inform object manager
	return index;
}

/** Removes the field at the given index. */
void
cld_object_remove (struct cld_object *object, int index)
{
	struct cld_object_field *field = &object->fields[index];
	if (field->object) cld_object_unref(field->object);
	cld_object_remove_field(object, index);
	
	//TODO: inform object manager
}

/** Removes the field with the given object. */
void
cld_object_remove_object (struct cld_object *object, struct cld_object *value)
{
	int index = cld_object_find(object, value);
	if (index < 0)
		return;
	cld_object_remove(object, index);
}

/** Returns the index of the field with the given object, or -1 if none was
 * found */
int
cld_object_find (struct cld_object *object, struct cld_object *value)
{
	int i;
	for (i = 0; i < object->num_fields; i++) {
		if (object->fields[i].object == value)
			return i;
	}
	return -1;
}

/** Returns the object of the field at the given index. */
struct cld_object *
cld_object_at (struct cld_object *object, int index)
{
	assert(index >= 0 && index < object->num_fields && "trying to get field index out of bounds");
	return object->fields[index].name;
}



//UNREVIEWED DEPRECATED STUFF
static void
print (struct cld_object *object, int indent)
{
	char in[128];
	int i;
	for (i = 0; i < indent * 4 && i < 127; i++)
		in[i] = ' ';
	in[i] = 0;
	
	//Null objects
	if (object == NULL) {
		printf("NULL");
	}
	
	//Strings
	else if (cld_object_is_string(object)) {
		printf("\"%s\"", object->str);
		return;
	}
	
	//Arrays
	else if (cld_object_is_array(object)) {
		printf("[");
		struct cld_field *field = object->field_head;
		int i = 0;
		while (field) {
			printf("\n%s    %i = ", in, i++);
			print(field->object, indent + 1);
			field = field->next;
		}
		printf("\n%s]", in);
	}
	
	//Objects
	else {
		printf("%s {", object->type);
		struct cld_field *field = object->field_head;
		while (field) {
			printf("\n%s    %s = ", in, field->name);
			print(field->object, indent + 1);
			field = field->next;
		}
		printf("\n%s}", in);
	}
}

/** Dumps the object to standard output. Nice for debugging. */
void
cld_object_print (struct cld_object *object)
{
	print(object, 0);
	printf("\n");
}

/* Serializes the given object into the buffer. The object may be NULL. */
static int
serialize (struct cld_object *object, struct cld_buffer *buffer)
{
	int length = 0;
	int offset = buffer->length;
	cld_buffer_put(buffer, &length, sizeof length);
	
	if (object) {
		cld_buffer_put(buffer, object->type, strlen(object->type) + 1);
		
		//Strings
		if (cld_object_is_string(object)) {
			cld_buffer_put(buffer, object->str, strlen(object->str) + 1);
		}
		
		//Arrays
		else if (cld_object_is_array(object)) {
			struct cld_field *field = object->field_head;
			while (field) {
				serialize(field->object, buffer);
				field = field->next;
			}
		}
		
		//Objects
		else {
			struct cld_field *field = object->field_head;
			while (field) {
				cld_buffer_put(buffer, field->name, strlen(field->name) + 1);
				serialize(field->object, buffer);
				field = field->next;
			}
		}
	}
	
	//Fix the length of the object.
	length = buffer->length - offset;
	*(int *)(buffer->data + offset) = length;
	
	return 0;
}

/** Serializes the object and returns a pointer to a newly allocated region of
 * memory containing the object. The caller is responsible for freeing the re-
 * turned memory after usage. */
struct cld_buffer *
cld_object_serialize (struct cld_object *object)
{
	struct cld_buffer *buffer = cld_buffer_create();
	if (buffer == NULL)
		return NULL;
	
	if (serialize(object, buffer) < 0) {
		cld_buffer_destroy(buffer);
		return NULL;
	}
	
	return buffer;
}


static struct cld_object *
unserialize (const void **data, size_t length)
{
	const void *base = *data;
	
	if (length < sizeof(int)) {
		fprintf(stderr, "%s: unexpected end of data\n", __FUNCTION__);
		return NULL;
	}
	
	int obj_length = *(int *)*data;
	*data += sizeof(int);
	if (obj_length > length) {
		fprintf(stderr, "%s: object of %u bytes, but only %lu bytes left in the buffer\n", __FUNCTION__, obj_length, length);
		return NULL;
	}
	
	const char *type = *data;
	for (; *(char *)*data != 0; (*data)++) {
		if (*data - base >= length) {
			fprintf(stderr, "%s: unexpected end of data when reading object type\n", __FUNCTION__);
			return NULL;
		}
	}
	(*data)++;
	
	//Unserialize Strings
	if (strcmp(type, CLD_OBJECT_STRING_TYPE) == 0) {
		const char *value = *data;
		for (; *(char *)*data != 0; (*data)++) {
			if (*data - base >= length) {
				fprintf(stderr, "%s: unexpected end of data when reading string\n", __FUNCTION__);
				return NULL;
			}
		}
		(*data)++;
		
		return cld_object_create_string(value);
	}
	
	//Unserialize Arrays
	else if (strcmp(type, CLD_OBJECT_ARRAY_TYPE) == 0) {
		struct cld_object *object = cld_object_create_array();
		if (object == NULL)
			return NULL;
		
		int i = 0;
		while (*data - base < obj_length) {
			struct cld_object *value = unserialize(data, length - (*data - base));
			if (value == NULL) {
				fprintf(stderr, "%s: unable to unserialize array element %i\n", __FUNCTION__, i);
				cld_object_destroy(object);
				return NULL;
			}
			i++;
			cld_object_array_add(object, value);
		}
		
		return object;
	}
	
	//Unserialize Objects
	else {
		struct cld_object *object = cld_object_create(type);
		if (object == NULL)
			return NULL;
		
		while (*data - base < obj_length) {
			const char *name = *data;
			for (; *(char *)*data != 0; (*data)++) {
				if (*data - base >= length) {
					fprintf(stderr, "%s: unexpected end of data when reading field name in object %s\n", __FUNCTION__, type);
					cld_object_destroy(object);
					return NULL;
				}
			}
			(*data)++;
			
			struct cld_object *value = unserialize(data, length - (*data - base));
			if (value == NULL) {
				fprintf(stderr, "%s: unable to unserialize field %s\n", __FUNCTION__, name);
				cld_object_destroy(object);
				return NULL;
			}
			
			cld_object_set(object, name, value);
		}
		
		return object;
	}
}

/** Reconstructs the object from the given bytes. */
struct cld_object *
cld_object_unserialize (void *buffer, size_t length)
{
	const void *data = buffer;
	return unserialize(&data, length);
}