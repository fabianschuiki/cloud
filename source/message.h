/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef MESSAGE_H
#define MESSAGE_H


enum {
	CLD_MSG_REQUEST_ID_RANGE,
	CLD_MSG_ID_RANGE,
	
	CLD_MSG_REQUEST_OBJECT,
	CLD_MSG_OBJECT,
	
	CLD_MSG_ADVERTISE,
	CLD_MSG_DESTROYED,
	
	CLD_MSG_COLLECTION_ADD,
	CLD_MSG_COLLECTION_REMOVE
};

struct cld_message {
	unsigned char op;
	union {
		struct {
			int id;
			const char *type;
			struct cld_object *object;
		} obj;
		struct {
			int min;
			int max;
		} id;
	};
};


struct cld_message *cld_message_create_advertise (int id, const char *type);
struct cld_message *cld_message_create_destroyed (int id);

void cld_message_destroy (struct cld_message *message);


#endif