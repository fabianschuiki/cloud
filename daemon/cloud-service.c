/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>

#include "cloud-service.h"
#include "socket.h"


struct cld_daemon {
	struct cld_socket *socket;
};

struct cld_daemon *
cld_daemon_connect ()
{
	struct cld_daemon *daemon;
	
	daemon = malloc(sizeof *daemon);
	if (daemon == NULL)
		return NULL;
	
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
	
	return daemon;
}

void
cld_daemon_disconnect (struct cld_daemon *daemon)
{
	cld_socket_destroy(daemon->socket);
	free(daemon);
}


struct cld_service {
	struct cld_daemon *daemon;
};


struct cld_service *
cld_service_create ()
{
	struct cld_service *service;
	
	service = malloc(sizeof *service);
	if (service == NULL)
		return NULL;
	
	service->daemon = cld_daemon_connect();
	if (service->daemon == NULL) {
		free(service);
		return NULL;
	}
	
	return service;
}

void
cld_service_destroy (struct cld_service *service)
{
	cld_daemon_disconnect(service->daemon);
	free(service);
}
