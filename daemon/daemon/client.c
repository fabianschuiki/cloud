/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "client.h"
#include "../daemon.h"
#include "../event-loop.h"
#include "../connection.h"
#include "../object.h"


/** Called by the event loop whenever there is traffic on the client's socket.
 * Routes the traffic through to the client's connection. May destroy the
 * client if a disconnect is detected. */
static int
socket_data (int fd, int mask, void *data)
{
	struct cld_client *client = data;
	
	int len = cld_connection_data(client->connection, mask);
	if (len < 0) {
		cld_client_destroy(client);
		return 0;
	}
	
	return len;
}

/** Called by the client's connection whenever a message is received. */
static int
message_received (struct cld_object *object, void *data)
{
	struct cld_client *client = data;
	printf("client %p sent ", client);
	cld_object_print(object);
	cld_object_destroy(object);
	return 1;
}

/** Creates a new client within the given daemon, communicating through fd. */
struct cld_client *
cld_client_create (struct cld_daemon *daemon, int fd)
{
	struct cld_client *client;
	
	client = malloc(sizeof *client);
	if (client == NULL)
		return NULL;
	
	memset(client, 0, sizeof *client);
	client->daemon = daemon;
	client->fd = fd;
	
	client->connection = cld_connection_create(fd, message_received, client);
	if (client->connection == NULL) {
		free(client);
		return NULL;
	}
	
	client->source = cld_event_loop_add_fd(daemon->loop, fd, CLD_EVENT_READABLE, socket_data, client);
	if (client->source == NULL) {
		cld_connection_destroy(client->connection);
		free(client);
		return NULL;
	}
	
	printf("client connected %p\n", client);
	
	return client;
}

/** Closes the connection, removes the socket from the event loop and frees the
 * memory occupied by the service. */
void
cld_client_destroy (struct cld_client *client)
{
	printf("client disconnected %p\n", client);
	cld_connection_destroy(client->connection);
	cld_event_source_remove(client->source);
	free(client);
}
