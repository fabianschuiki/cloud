/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "util.h"
#include "connection.h"
#include "event-loop.h"
#include "buffer.h"
#include "object.h"


struct cld_connection {
	int fd;
	cld_connection_received_func_t received;
	void *data;
	
	struct cld_buffer *inbuf;
	struct cld_buffer *outbuf;
};

struct cld_message {
	int op;
	size_t length;
};


struct cld_connection *
cld_connection_create (int fd, cld_connection_received_func_t received, void *data)
{
	struct cld_connection *connection;
	
	connection = malloc(sizeof *connection);
	if (connection == NULL)
		return NULL;
	
	connection->fd = fd;
	connection->received = received;
	connection->data = data;
	
	connection->inbuf = cld_buffer_create();
	connection->outbuf = cld_buffer_create();
	if (connection->inbuf == NULL || connection->outbuf == NULL) {
		free(connection);
		return NULL;
	}
	
	return connection;
}

void
cld_connection_destroy (struct cld_connection *connection)
{
	cld_buffer_destroy(connection->inbuf);
	cld_buffer_destroy(connection->outbuf);
	free(connection);
}

struct cld_object *
read_object (struct cld_connection *connection, int block)
{
	char buffer[4096];
	int len = read(connection->fd, buffer, 4096);
	if (len <= 0) {
		close(connection->fd);
		connection->fd = 0;
		return NULL;
	}
	cld_buffer_put(connection->inbuf, buffer, len);
	
	if (connection->inbuf->length >= sizeof(int)) {
		int length = *(int *)connection->inbuf->data;
		if (connection->inbuf->length < length)
			return NULL;
		
		void *data = cld_buffer_slice(connection->inbuf, length);
		struct cld_object *object = cld_object_unserialize(data, length);
		free(data);
		
		if (object)
			return object;
	}
	
	return NULL;
}

int
cld_connection_data (struct cld_connection *connection, int mask)
{
	if (mask & CLD_EVENT_WRITABLE) {
		printf("updating writable connection\n");
		//connection->update(connection, CLD_EVENT_READABLE, connection->data);
		return 0;
	}
	
	if (mask & CLD_EVENT_READABLE) {
		struct cld_object *object;
		while (object = read_object(connection, 0))
			connection->received(object, connection->data);
		return connection->fd > 0 ? 0 : -1;
	}
}

int
cld_connection_write (struct cld_connection *connection, struct cld_object *object)
{
	struct cld_buffer *buffer = cld_object_serialize(object);
	if (buffer == NULL)
		return -1;
	
	if (write(connection->fd, buffer->data, buffer->length) < 0) {
		error("write");
		cld_buffer_destroy(buffer);
		return -1;
	}
	
	cld_buffer_destroy(buffer);
	return 0;
}

struct cld_object *
cld_connection_read (struct cld_connection *connection)
{
	struct cld_object *object = read_object(connection, 1);
	return object;
}
