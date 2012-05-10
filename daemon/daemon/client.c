/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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

static void
send_error (struct cld_client *client, const char *msg, ...)
{
	va_list list;
	va_start(list, msg);
	char *formatted;
	vasprintf(&formatted, msg, list);
	va_end(list);
	
	printf("client %p produced error: %s\n", client, formatted);
	
	struct cld_object *error = cld_object_create("error");
	if (error == NULL) {
		fprintf(stderr, "%s: unable to create error object for message '%s'\n", __FUNCTION__, formatted);
		return;
	}
	
	cld_object_set(error, "message", cld_object_create_string(formatted));
	cld_connection_write(client->connection, error);
	cld_object_destroy(error);
}

/** Called by message_received whenever a 'request' object was received that
 * needs to be handled. */
static int
handle_request (struct cld_client *client, struct cld_object *object)
{
	const char *type = cld_object_get_string(object, "object");
	if (type == NULL) {
		send_error(client, "request: specify what you're requesting in the 'object' field");
		return -1;
	}
	
	if (strcmp(type, "accounts") == 0) {
		struct cld_object *accounts = cld_object_create_array();
		if (accounts == NULL)
			return -1;
		
		struct cld_object *account = cld_object_create("account");
		cld_object_set(account, "type", cld_object_create_string("twitter"));
		cld_object_set(account, "username", cld_object_create_string("fabianschuiki"));
		cld_object_set(account, "token", cld_object_create_string("assdth684dsrhdfzk84sdrh"));
		cld_object_set(account, "uuid", cld_object_create_string("925b7d25"));
		cld_object_array_add(accounts, account);
		
		account = cld_object_create("account");
		cld_object_set(account, "type", cld_object_create_string("wordpress"));
		cld_object_set(account, "username", cld_object_create_string("fabianschuiki"));
		cld_object_set(account, "password", cld_object_create_string("my_wordpress_password"));
		cld_object_set(account, "uuid", cld_object_create_string("ebd47106"));
		cld_object_array_add(accounts, account);
		
		cld_connection_write(client->connection, accounts);
		cld_object_destroy(accounts);
		return 0;
	}
	
	send_error(client, "request: requested object '%s' is unknown", type);
	return -1;
}

/** Called by the client's connection whenever a message is received. */
static int
message_received (struct cld_object *object, void *data)
{
	struct cld_client *client = data;
	printf("client %p sent ", client);
	cld_object_print(object);
	
	int result = 0;
	if (cld_object_is(object, "request")) {
		if (handle_request(client, object) < 0) {
			send_error(client, "unable to handle request");
			cld_object_print(object);
			result = -1;
		}
	}
	
	cld_object_destroy(object);
	return result;
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
