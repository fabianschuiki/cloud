/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>

#include "client.h"
#include "socket.h"
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
	
	client->daemon = cld_daemon_connect(client);
	if (client->daemon == NULL) {
		fprintf(stderr, "%s: unable to connect to cloud daemon\n", __FUNCTION__);
		free(client);
		return NULL;
	}
	
	return client;
}

void
cld_client_destroy (struct cld_client *client)
{
	cld_daemon_disconnect(client->daemon);
	free(client);
}

static int
catch_error (const char *func, struct cld_object *response)
{
	if (cld_object_is(response, "error")) {
		fprintf(stderr, "%s: daemon returned error: %s\n", func, cld_object_string(cld_object_get(response, "message")));
		return -1;
	}
	return 0;
}

void
cld_client_account_set (struct cld_client *client, struct cld_object *account)
{
	cld_connection_write_blocking(client->daemon->connection, account);
}

struct cld_object *
cld_client_account_list (struct cld_client *client)
{
	struct cld_object *req = cld_object_create("request");
	if (req == NULL)
		return NULL;
	
	cld_object_set(req, "object", cld_object_create_string("accounts"));
	
	if (cld_connection_write_blocking(client->daemon->connection, req) < 0)
		return NULL;
	
	struct cld_object *response = cld_connection_read_blocking(client->daemon->connection);
	if (response == NULL)
		return NULL;
	if (catch_error(__FUNCTION__, response) < 0)
		return NULL;
	
	return response;
}
