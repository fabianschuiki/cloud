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


struct cld_buffer {
	void *data;
	int length;
	size_t size;
};

struct cld_buffer *
cld_buffer_create ()
{
	struct cld_buffer *buffer;
	
	buffer = malloc(sizeof *buffer);
	if (buffer == NULL)
		return NULL;
	
	buffer->length = 0;
	buffer->size = 1024;
	buffer->data = malloc(buffer->size);
	
	return buffer;
}

void
cld_buffer_destroy (struct cld_buffer *buffer)
{
	free(buffer->data);
	free(buffer);
}

void
cld_buffer_resize (struct cld_buffer *buffer, size_t size)
{
	printf("resizing buffer %p to %d\n", buffer, (int)size);
	void *data = malloc(size);
	memcpy(data, buffer->data, buffer->size);
	free(buffer->data);
	buffer->data = data;
}

void
cld_buffer_extend (struct cld_buffer *buffer, size_t size)
{
	size_t new_size = buffer->size;
	while (new_size < buffer->length + size) new_size <<= 1;
	
	if (new_size != buffer->size)
		cld_buffer_resize(buffer, new_size);
}

void
cld_buffer_put (struct cld_buffer *buffer, void *data, size_t length)
{
	cld_buffer_extend(buffer, length);
	memcpy(buffer->data + buffer->length, data, length);
	buffer->length += length;
}

void *
cld_buffer_slice (struct cld_buffer *buffer, size_t offset)
{
	printf("slicing buffer %p at %lu\n", buffer, offset);
	void *data = buffer->data;
	buffer->data = malloc(buffer->size);
	buffer->length -= offset;
	memcpy(buffer->data, data + offset, buffer->length);
	return data;
}


struct cld_connection {
	int fd;
	cld_connection_message_func_t message;
	void *data;
	
	struct cld_buffer *inbuf;
	struct cld_buffer *outbuf;
};

struct cld_message {
	int op;
	size_t length;
};


struct cld_connection *
cld_connection_create (int fd, cld_connection_message_func_t message, void *data)
{
	struct cld_connection *connection;
	
	connection = malloc(sizeof *connection);
	if (connection == NULL)
		return NULL;
	
	connection->fd = fd;
	connection->message = message;
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

int
cld_connection_data (struct cld_connection *connection, int mask)
{
	if (mask & CLD_EVENT_WRITABLE) {
		printf("updating writable connection\n");
		//connection->update(connection, CLD_EVENT_READABLE, connection->data);
		return 0;
	}
	
	if (mask & CLD_EVENT_READABLE) {
		char buffer[4096];
		int len = read(connection->fd, buffer, 4096);
		if (len <= 0)
			return -1;
		cld_buffer_put(connection->inbuf, buffer, len);
		
		printf("read %d bytes, %d in the buffer\n", len, connection->inbuf->length);
		
		struct cld_message *msg = connection->inbuf->data;
		while (connection->inbuf->length >= sizeof *msg) {
			size_t length = msg->length + sizeof *msg;
			if (connection->inbuf->length < length)
				break;
			
			void *data = cld_buffer_slice(connection->inbuf, length);
			connection->message(msg->op, data + sizeof *msg, msg->length, connection->data);
			free(data);
		}
		
		return len;
	}
}

int
cld_connection_write (struct cld_connection *connection,
	int op,
	const void *data,
	size_t length)
{
	struct cld_message msg;
	msg.op = op;
	msg.length = length;
	printf("writing op %d of length %lu\n", op, length);
	if (write(connection->fd, &msg, sizeof msg) < 0) {
		error("write header");
		return -1;
	}
	if (length == 0)
		return 0;
	
	if (write(connection->fd, data, length) < 0) {
		error("write data");
		return -1;
	}
	return 0;
}
