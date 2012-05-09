/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>

#include "client.h"
#include "socket.h"
#include "event-loop.h"
#include "client/daemon.h"


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
