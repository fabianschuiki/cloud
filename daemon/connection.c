/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "util.h"
#include "connection.h"
#include "fd-public.h"
//#include "event-loop.h"
#include "buffer.h"
#include "object.h"


struct cld_connection *
cld_connection_create (int fd, cld_connection_received_func_t received, cld_connection_disconnected_func_t disconnected, void *data)
{
	struct cld_connection *connection;
	
	connection = malloc(sizeof *connection);
	if (connection == NULL)
		return NULL;
	
	connection->fd = fd;
	connection->received = received;
	connection->disconnected = disconnected;
	connection->data = data;
	connection->mask = CLD_FD_READ;
	
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
parse_object (struct cld_connection *connection)
{
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
cld_connection_communicate (struct cld_connection *connection, int dir)
{
	printf("%s: connection %p, mask = %x\n", __FUNCTION__, connection, dir);
	
	if (dir & CLD_FD_WRITE) {
		ssize_t num = write(connection->fd, connection->outbuf->data, connection->outbuf->length);
		if (num < 0) {
			error("write()");
			return -1;
		}
		
		void *sent = cld_buffer_slice(connection->outbuf, num);
		free(sent);
		
		if (connection->outbuf->length == 0)
			connection->mask &= ~CLD_FD_WRITE;
		
		return 0;
	}
	
	if (dir & CLD_FD_READ) {
		char buffer[4096];
		ssize_t num = read(connection->fd, buffer, 4096);
		if (num < 0) {
			error("read()");
			return -1;
		}
		if (num == 0) {
			printf("connection %p peer hung up\n", connection);
			close(connection->fd);
			connection->fd = 0;
			connection->disconnected(connection->data);
			return 0;
		}
		
		cld_buffer_put(connection->inbuf, buffer, num);
		
		return 0;
	}
}

/** Parses the connection's input buffer into objects. For each object parsed,
 * the connection's received handler is called.
 * 
 * @returns the number of objects parsed, or -1 upon failure */
int
cld_connection_parse_objects (struct cld_connection *connection)
{
	struct cld_object *object;
	int num = 0;
	while (object = parse_object(connection)) {
		num++;
		connection->received(object, connection->data);
	}
	return num;
}


/** Serializes the given object and queues it up for delivery on the given
 * connection. The function returns immediately. */
int
cld_connection_write (struct cld_connection *connection, struct cld_object *object)
{
	struct cld_buffer *buffer = cld_object_serialize(object);
	if (buffer == NULL)
		return -1;
	
	cld_buffer_put(connection->outbuf, buffer->data, buffer->length);
	connection->mask |= CLD_FD_WRITE;
	
	return 0;
}

/** Serializes the given object and delivers it immediately on the given
 * connection. The function blocks until the object was delivered. */
int
cld_connection_write_blocking (struct cld_connection *connection, struct cld_object *object)
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

/** Reads one object from the connection. The function blocks until an object
 * was read. */
struct cld_object *
cld_connection_read_blocking (struct cld_connection *connection)
{
	struct cld_object *object = parse_object(connection);
	while (object == NULL) {
		if (cld_connection_communicate(connection, CLD_FD_READ) < 0)
			return NULL;
		object = parse_object(connection);
	}
	return object;
}
