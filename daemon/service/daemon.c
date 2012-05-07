/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>

#include "daemon.h"
#include "../cloud-service-private.h"
#include "../socket.h"
#include "../event-loop.h"


struct cld_daemon {
	struct cld_service *service;
	struct cld_socket *socket;
	struct cld_event_source *source;
};

static int
socket_data (int fd, int mask, void *data)
{
	struct cld_daemon *daemon = data;
	
	int len = 0;
	//TODO: communicate through connection here.
	
	return len;
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
