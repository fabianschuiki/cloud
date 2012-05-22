/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "message.h"
#include "buffer.h"


struct cld_message *
cld_message_create (unsigned char op)
{
	struct cld_message *message;
	
	message = malloc(sizeof *message);
	if (message == NULL)
		return NULL;
	
	memset(message, 0, sizeof *message);
	message->op = op;
	
	return message;
}

struct cld_message *
cld_message_create_advertise (int id, const char *type)
{
	assert(type && "object type required");
	
	struct cld_message *message = cld_message_create(CLD_MSG_ADVERTISE);
	if (message != NULL) {
		message->obj.id = id;
		message->obj.type = strdup(type);
	}
	return message;
}

struct cld_message *
cld_message_create_destroyed (int id)
{
	struct cld_message *message = cld_message_create(CLD_MSG_DESTROYED);
	if (message != NULL) {
		message->obj.id = id;
	}
	return message;
}

void
cld_message_destroy (struct cld_message *message)
{
	if (message->op == CLD_MSG_ADVERTISE)
		free(message->obj.type);
	free(message);
}


struct cld_buffer *
cld_message_serialize (struct cld_message *message)
{
	struct cld_buffer *buffer = cld_buffer_create();
	if (buffer == 0)
		return NULL;
	
	cld_buffer_put(buffer, &message->op, sizeof message->op);
	if (message->op != CLD_MSG_ID_RANGE && message->op != CLD_MSG_REQUEST_ID_RANGE)
		cld_buffer_put(buffer, &message->obj.id, sizeof message->obj.id);
	if (message->op == CLD_MSG_ADVERTISE)
		cld_buffer_put(buffer, message->obj.type, strlen(message->obj.type) + 1);
	
	return buffer;
}

struct cld_message *
cld_message_unserialize (void *data, size_t length)
{
	struct cld_message *message;
	
	message = malloc(sizeof *message);
	if (message == NULL)
		return NULL;
	
	//TODO: add code that checks for length violations so the connection doesn't segfault on malformed messages.
	void *ptr = data;
	message->op = *(unsigned char *)ptr; ptr += sizeof message->op;
	if (message->op != CLD_MSG_ID_RANGE && message->op != CLD_MSG_REQUEST_ID_RANGE)
		message->obj.id = *(int *)ptr; ptr += sizeof message->obj.id;
	if (message->op == CLD_MSG_ADVERTISE) {
		message->obj.type = strdup(ptr);
		ptr += strlen(ptr) + 1;
	}
	
	return message;
}


#define OPCASE(suffix) case CLD_MSG_ ## suffix: op = #suffix; break;

void
cld_message_print (struct cld_message *message)
{
	const char *op = NULL;
	switch (message->op) {
		OPCASE(SYNC)
		OPCASE(ADVERTISE)
		OPCASE(DESTROYED)
	}
	
	printf("%s", op);
	
	if (message->op < _CLD_MSG_MAX_TYPE_SYNC) {
		printf(" %i", message->sync);
	}
	else if (message->op < _CLD_MSG_MAX_TYPE_ID) {
		printf(" %i .. %i", message->id.min, message->id.max);
	}
	else if (message->op < _CLD_MSG_MAX_TYPE_OBJ) {
		printf(" %i %s", message->obj.id, message->obj.type);
	}
}