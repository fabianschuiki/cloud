/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "util.h"
#include "private.h"


struct cld_connection {
	int fd;
	cld_connection_update_func_t update;
	void *data;
};


struct cld_connection *
cld_connection_create (int fd, cld_connection_update_func_t update,  void *data)
{
	struct cld_connection *connection;
	
	connection = malloc(sizeof *connection);
	if (connection == NULL)
		return NULL;
	
	connection->fd = fd;
	connection->update = update;
	connection->data = data;
	
	return connection;
}

void
cld_connection_destroy (struct cld_connection *connection)
{
	free(connection);
}

int
cld_connection_data (struct cld_connection *connection, int mask)
{
	if (mask & CLD_CONNECTION_WRITABLE) {
		printf("updating writable connection\n");
		connection->update(connection, CLD_CONNECTION_READABLE, connection->data);
		return 0;
	}
	
	if (mask & CLD_CONNECTION_READABLE) {
		printf("received data\n");
		char buffer[1024];
		int len = read(connection->fd, buffer, 1023);
		buffer[len] = 0;
		printf(" %s\n", buffer);
		if (len <= 0) return -1;
		return len;
	}
}

int
cld_connection_write (struct cld_connection *connection,
	const void *data,
	size_t length)
{
	if (write(connection->fd, data, length) < 0) {
		error("write");
		return -1;
	}
	return 0;
}
