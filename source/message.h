/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdlib.h>


struct cld_buffer;

enum {
	CLD_MSG_SYNC,
	
	_CLD_MSG_MAX_TYPE_SYNC,
	
	
	CLD_MSG_REQUEST_ID_RANGE,
	CLD_MSG_ID_RANGE,
	
	_CLD_MSG_MAX_TYPE_ID,
	
	
	CLD_MSG_REQUEST_OBJECT,
	CLD_MSG_OBJECT,
	
	CLD_MSG_ADVERTISE,
	CLD_MSG_DESTROYED,
	
	CLD_MSG_COLLECTION_ADD,
	CLD_MSG_COLLECTION_REMOVE,
	
	_CLD_MSG_MAX_TYPE_OBJ
};

struct cld_message {
	unsigned char op;
	struct cld_connection *connection;
	union {
		int sync;
		struct {
			int id;
			char *type;
			struct cld_object *object;
		} obj;
		struct {
			int min;
			int max;
		} id;
	};
};

struct cld_message *cld_message_create (unsigned char op);
struct cld_message *cld_message_create_advertise (int id, const char *type);
struct cld_message *cld_message_create_destroyed (int id);

void cld_message_destroy (struct cld_message *message);

struct cld_buffer *cld_message_serialize (struct cld_message *message);
struct cld_message *cld_message_unserialize (void *data, size_t length);

void cld_message_print (struct cld_message *message);


#endif