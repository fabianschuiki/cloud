/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "daemon.h"
#include "../cloud-service-private.h"
#include "../socket.h"
#include "../event-loop.h"
#include "../connection.h"
#include "../protocol.h"


struct cld_daemon {
	struct cld_service *service;
	struct cld_socket *socket;
	struct cld_event_source *source;
	struct cld_connection *connection;
};

static int
socket_data (int fd, int mask, void *data)
{
	struct cld_daemon *daemon = data;
	
	int len = cld_connection_data(daemon->connection, mask);
	if (len < 0) {
		cld_daemon_disconnect(daemon);
		return 0;
	}
	
	return len;
}

static int
message_received (int op, void *message, size_t length, void *data)
{
	struct cld_service *daemon = data;
	printf("daemon %p sent message %d of %d bytes\n", daemon, op, (int)length);
	return 1;
}

struct cld_daemon *
cld_daemon_connect (struct cld_service *service)
{
	struct cld_daemon *daemon;
	
	daemon = malloc(sizeof *daemon);
	if (daemon == NULL)
		return NULL;
	
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
	
	daemon->source = cld_event_loop_add_fd(service->loop, cld_socket_get_fd(daemon->socket), CLD_EVENT_READABLE, socket_data, daemon);
	if (daemon->source == NULL) {
		cld_socket_destroy(daemon->socket);
		free(daemon);
		return NULL;
	}
	
	return daemon;
}

void
cld_daemon_disconnect (struct cld_daemon *daemon)
{
	cld_socket_destroy(daemon->socket);
	free(daemon);
}


int
cld_daemon_send_service_record (struct cld_daemon *daemon, const char *name)
{
	struct cld_service_record record;
	record.name_len = strlen(name);
	
	size_t len = sizeof record + record.name_len;
	void *data = malloc(len);
	void *ptr = data;
	memcpy(ptr, &record, sizeof record); ptr += sizeof record;
	memcpy(ptr, name, record.name_len); ptr += record.name_len;
	
	return cld_connection_write(daemon->connection, CLD_OP_SERVICE_RECORD, data, len);
}
