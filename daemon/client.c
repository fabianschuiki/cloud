/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>

#include "client.h"
#include "socket.h"
#include "event-loop.h"
#include "client/daemon.h"
#include "connection.h"
#include "object.h"


struct cld_client *
cld_client_create ()
{
	struct cld_client *client;
	
	client = malloc(sizeof *client);
	if (client == NULL)
		return NULL;
	
	client->loop = cld_event_loop_create();
	if (client->loop == NULL) {
		free(client);
		return NULL;
	}
	
	client->daemon = cld_daemon_connect(client);
	if (client->daemon == NULL) {
		fprintf(stderr, "%s: unable to connect to cloud daemon\n", __FUNCTION__);
		cld_event_loop_destroy(client->loop);
		free(client);
		return NULL;
	}
	
	return client;
}

void
cld_client_destroy (struct cld_client *client)
{
	cld_daemon_connect(client->daemon);
	cld_event_loop_destroy(client->loop);
	free(client);
}

void
cld_client_account_set (struct cld_client *client, struct cld_object *account)
{
	cld_connection_write(client->daemon->connection, account);
}

struct cld_object *
cld_client_account_list (struct cld_client *client)
{
	struct cld_object *req = cld_object_create("request");
	if (req == NULL)
		return NULL;
	
	cld_object_set(req, "object", cld_object_create_string("accounts"));
	
	if (cld_connection_write(client->daemon->connection, req) < 0)
		return NULL;
	
	return NULL;
}
