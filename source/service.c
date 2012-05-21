/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "service.h"
#include "socket.h"
#include "connection.h"
#include "service/daemon.h"
#include "fd-public.h"
#include "object.h"


static int
connection_received (struct cld_object *object, void *data)
{
	struct cld_service *service = data;
	printf("daemon sent ", service);
	cld_object_print(object);
	cld_object_destroy(object);
	return 1;
}

static void
connection_disconnected (void *data)
{
	fprintf(stderr, "*** daemon disconnect not handled\n");
}


struct cld_service *
cld_service_create ()
{
	struct cld_service *service;
	
	service = malloc(sizeof *service);
	if (service == NULL)
		return NULL;
	
	memset(service, 0, sizeof *service);
	
	service->socket = cld_socket_create(CLD_SOCKET_SERVICE);
	if (service->socket == NULL) {
		free(service);
		return NULL;
	}
	
	if (cld_socket_connect(service->socket) < 0) {
		cld_socket_destroy(service->socket);
		free(service);
		return NULL;
	}
	
	service->connection = cld_connection_create(cld_socket_get_fd(service->socket), connection_received, connection_disconnected, service);
	if (service->connection == NULL) {
		cld_socket_destroy(service->socket);
		free(service);
		return NULL;
	}
	
	return service;
}

void
cld_service_destroy (struct cld_service *service)
{
	free(service);
}

/** Returns the file descriptors the service uses for communication, so that
 * the caller may perform some form of fd multiplexing, e.g. by using select()
 * or poll() with the returned fds. */
struct cld_fd *
cld_service_get_fds (struct cld_service *service, int *count)
{
	struct cld_fd *fds;
	*count = 1;
	
	fds = malloc(*count * sizeof(*fds));
	if (fds == NULL)
		return NULL;
	
	fds[0].fd = service->connection->fd;
	fds[0].mask = service->connection->mask;
	
	return fds;
}

void cld_service_communicate(struct cld_service *service, struct cld_fd *fds, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (service->connection->fd == fds[i].fd)
			cld_connection_communicate(service->connection, fds[i].mask);
	}
}

void
cld_service_set_name (struct cld_service *service, const char *name)
{
	service->name = name;
}

void
cld_service_send_descriptor (struct cld_service *service)
{
	/*struct cld_object *desc = cld_object_create("service");
	
	cld_object_set(desc, "name", cld_object_create_string(service->name));
	
	cld_connection_write(service->connection, desc);*/
}
