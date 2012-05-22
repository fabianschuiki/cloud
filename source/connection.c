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
#include "buffer.h"
#include "object.h"
#include "message.h"


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

struct cld_message *
parse_message (struct cld_connection *connection)
{
	if (connection->inbuf->length >= sizeof(int)) {
		int length = *(int *)connection->inbuf->data;
		if (connection->inbuf->length < length)
			return NULL;
		
		void *data = cld_buffer_slice(connection->inbuf, length);
		struct cld_message *message = cld_message_unserialize(data, length);
		free(data);
		
		if (message)
			return message;
	}
	
	return NULL;
}

static int
communicate (struct cld_connection *connection, int mask)
{
	if (mask & CLD_FD_WRITE) {
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
	
	if (mask & CLD_FD_READ) {
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
			return -1;
		}
		
		cld_buffer_put(connection->inbuf, buffer, num);
		
		return 0;
	}
}

int
cld_connection_communicate (struct cld_connection *connection, int dir)
{
	int retval = communicate(connection, dir);
	if (retval == 0 && dir & CLD_FD_READ)
		cld_connection_parse(connection);
	return retval;
}

/** Parses the connection's input buffer into objects. For each object parsed,
 * the connection's received handler is called.
 * 
 * @returns the number of objects parsed, or -1 upon failure */
int
cld_connection_parse (struct cld_connection *connection)
{
	struct cld_message *message;
	int num = 0;
	while (message = parse_message(connection)) {
		num++;
		connection->received(message, connection->data);
	}
	return num;
}


/** Serializes the given object and queues it up for delivery on the given
 * connection. The function returns immediately. */
int
cld_connection_write (struct cld_connection *connection, struct cld_message *message)
{
	struct cld_buffer *buffer = cld_message_serialize(message);
	if (buffer == NULL)
		return -1;
	int length = buffer->length;
	
	cld_buffer_put(connection->outbuf, &length, sizeof length);
	cld_buffer_put(connection->outbuf, buffer->data, buffer->length);
	connection->mask |= CLD_FD_WRITE;
	
	cld_buffer_destroy(buffer);
	
	return 0;
}

/** Sends a sync message and consumes incoming messages until a sync message
 * is received. */
void
cld_connection_sync (struct cld_connection *connection)
{
	struct cld_message *message = cld_message_create(CLD_MSG_SYNC);
	cld_connection_write(connection, message);
	cld_message_destroy(message);
	
	while (message = parse_message(connection)) {
		if (message->op == CLD_MSG_SYNC) {
			cld_message_destroy(message);
			return;
		}
		connection->received(message, connection->data);
		cld_message_destroy(message);
	}
}