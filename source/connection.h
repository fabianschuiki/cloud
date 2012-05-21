/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>


//TODO: New message types. Implement this.
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

//TODO: new message container. Implement this.
struct cld_message {
	unsigned char op;
	union {
		struct {
			int id;
			char *type;
		} obj;
		struct {
			int min;
			int max;
		} id;
	};
};


struct cld_object;
struct cld_buffer;
typedef int (*cld_connection_received_func_t) (struct cld_object *object, void *data);
typedef void (*cld_connection_disconnected_func_t) (void *data);

struct cld_connection {
	int fd;
	int mask;
	cld_connection_received_func_t received;
	cld_connection_disconnected_func_t disconnected;
	void *data;
	
	struct cld_buffer *inbuf;
	struct cld_buffer *outbuf;
};

struct cld_connection *cld_connection_create (int fd, cld_connection_received_func_t received, cld_connection_disconnected_func_t disconnected, void *data);
void cld_connection_destroy (struct cld_connection *connection);

int cld_connection_communicate (struct cld_connection *connection, int mask);
int cld_connection_write(struct cld_connection *connection, struct cld_object *object);

int cld_connection_write_blocking (struct cld_connection *connection, struct cld_object *object);
struct cld_object *cld_connection_read_blocking (struct cld_connection *connection);


#endif