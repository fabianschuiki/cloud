/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "client.h"
#include "../socket.h"
#include "daemon.h"
#include "../connection.h"
#include "../object.h"
#include "../list.h"
#include "../account.h"


static int
connection_received (struct cld_object *object, void *data)
{
	struct cld_client *client = data;
	printf("daemon sent ", client);
	cld_object_print(object);
	return 0;
}

static void
connection_disconnected (void *data)
{
	fprintf(stderr, "*** daemon disconnect not handled\n");
}


struct cld_client *
cld_client_create ()
{
	struct cld_client *client;
	
	client = malloc(sizeof *client);
	if (client == NULL)
		return NULL;
	
	memset(client, 0, sizeof *client);
	
	client->socket = cld_socket_create(CLD_SOCKET_CLIENT);
	if (client->socket == NULL) {
		free(client);
		return NULL;
	}
	
	if (cld_socket_connect(client->socket) < 0) {
		cld_socket_destroy(client->socket);
		free(client);
		return NULL;
	}
	
	client->connection = cld_connection_create(cld_socket_get_fd(client->socket), connection_received, connection_disconnected, client);
	if (client->connection == NULL) {
		cld_socket_destroy(client->socket);
		free(client);
		return NULL;
	}
	
	return client;
}

void
cld_client_destroy (struct cld_client *client)
{
	cld_connection_destroy(client->connection);
	cld_socket_destroy(client->socket);
	
	if (client->accounts)
		cld_list_destroy(client->accounts);
	
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


static void
account_commit (struct cld_account *account, void *data)
{
	struct cld_client *client = data;
	
	printf("committing account %p\n", account);
	
	struct cld_object *object = cld_account_get_object(account);
	cld_object_print(object);
	if (cld_connection_write_blocking(client->connection, object) < 0)
		return;
}

struct cld_account *cld_client_add_account (struct cld_client *client, const char *type)
{
	struct cld_object *object;
	
	object = cld_object_create("make.account");
	if (object == NULL)
		return NULL;
	
	cld_object_set(object, "type", cld_object_create_string(type));
	
	if (cld_connection_write_blocking(client->connection, object) < 0) {
		cld_object_destroy(object);
		return NULL;
	}
	cld_object_destroy(object);
	
	struct cld_object *response = cld_connection_read_blocking(client->connection);
	if (response == NULL || catch_error(__FUNCTION__, response) < 0)
		return NULL;
	
	struct cld_account *account;
	
	account = cld_account_create(response, account_commit, client);
	if (account == NULL) {
		cld_object_destroy(response);
		return NULL;
	}
	
	return account;
}

static void
fetch_accounts (struct cld_client *client)
{
	printf("fetching accounts\n");
	
	struct cld_object *req = cld_object_create("request");
	if (req == NULL)
		return;
	
	cld_object_set(req, "object", cld_object_create_string("accounts"));
	
	if (cld_connection_write_blocking(client->connection, req) < 0)
		return;
	
	struct cld_object *response = cld_connection_read_blocking(client->connection);
	if (response == NULL)
		return;
	if (catch_error(__FUNCTION__, response) < 0)
		return;
	
	client->accounts = cld_list_create();
	if (client->accounts == NULL)
		return;
	
	int i;
	int num_accounts = cld_object_array_count(response);
	
	printf("received %i accounts\n", num_accounts);
	
	for (i = 0; i < num_accounts; i++) {
		struct cld_account *account = cld_account_create(cld_object_copy(cld_object_array_get(response, i)), account_commit, client);
		cld_list_add(client->accounts, account);
	}
	cld_object_destroy(response);
}

struct cld_account *
cld_client_get_account (struct cld_client *client, const char *id)
{
	if (client->accounts == NULL)
		fetch_accounts(client);
	
	struct cld_list_element *element = cld_list_begin(client->accounts);
	for (; element; element = cld_list_next(element)) {
		if (strcmp(cld_account_get_id(element->object), id) == 0)
			return element->object;
	}
	
	return NULL;
}


void
cld_client_account_set (struct cld_client *client, struct cld_object *account)
{
	cld_connection_write_blocking(client->connection, account);
}

struct cld_object *
cld_client_account_list (struct cld_client *client)
{
	struct cld_object *req = cld_object_create("request");
	if (req == NULL)
		return NULL;
	
	cld_object_set(req, "object", cld_object_create_string("accounts"));
	
	if (cld_connection_write_blocking(client->connection, req) < 0)
		return NULL;
	
	struct cld_object *response = cld_connection_read_blocking(client->connection);
	if (response == NULL)
		return NULL;
	if (catch_error(__FUNCTION__, response) < 0)
		return NULL;
	
	return response;
}
