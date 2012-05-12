/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "daemon.h"
#include "../service.h"
#include "../socket.h"
#include "../connection.h"
#include "../client.h"
#include "../object.h"


static int
socket_data (int fd, int mask, void *data)
{
	struct cld_daemon *daemon = data;
	
	int len = cld_connection_communicate(daemon->connection, mask);
	if (len < 0) {
		cld_daemon_disconnect(daemon);
		return 0;
	}
	
	return len;
}

static int
message_received (struct cld_object *object, void *data)
{
	struct cld_service *daemon = data;
	printf("daemon %p sent ", daemon);
	cld_object_print(object);
	cld_object_destroy(object);
	return 1;
}

struct cld_daemon *
cld_daemon_connect (struct cld_service *service)
{
	struct cld_daemon *daemon;
	
	daemon = malloc(sizeof *daemon);
	if (daemon == NULL)
		return NULL;
	
	memset(daemon, 0, sizeof *daemon);
	daemon->service = service;
	
	daemon->socket = cld_socket_create(CLD_SOCKET_SERVICE);
	if (daemon->socket == NULL) {
		free(daemon);
		return NULL;
	}
	
	if (cld_socket_connect(daemon->socket) < 0) {
		cld_socket_destroy(daemon->socket);
		free(daemon);
		return NULL;
	}
	
	daemon->connection = cld_connection_create(cld_socket_get_fd(daemon->socket), message_received, daemon);
	if (daemon->connection == NULL) {
		cld_socket_destroy(daemon->socket);
		free(daemon);
		return NULL;
	}
	
	/*daemon->source = cld_event_loop_add_fd(service->loop, cld_socket_get_fd(daemon->socket), CLD_EVENT_READABLE, socket_data, daemon);
	if (daemon->source == NULL) {
		cld_socket_destroy(daemon->socket);
		free(daemon);
		return NULL;
	}*/
	
	return daemon;
}

void
cld_daemon_disconnect (struct cld_daemon *daemon)
{
	//cld_event_source_remove(daemon->source);
	cld_connection_destroy(daemon->connection);
	cld_socket_destroy(daemon->socket);
	free(daemon);
}


int
cld_daemon_send_service_record (struct cld_daemon *daemon, const char *name)
{
	struct cld_object *record = cld_object_create("service");
	if (record == NULL)
		return -1;
	
	cld_object_set(record, "name", cld_object_create_string(name));
	
	cld_connection_write(daemon->connection, record);
	cld_object_destroy(record);
}
