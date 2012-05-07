/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>

#include "service.h"
#include "../cloud-daemon.h"
#include "../event-loop.h"
#include "../connection.h"


/** Called by the event loop whenever there is traffic on the service's socket.
 * Routes the traffic through to the service's connection. May destroy the
 * service if a disconnect is detected. */
static int
socket_data (int fd, int mask, void *data)
{
	struct cld_service *service = data;
	
	int len = cld_connection_data(service->connection, mask);
	if (len < 0) {
		cld_service_destroy(service);
		return 0;
	}
	
	return len;
}

/** Called by the service's connection whenever a message is received. */
static int
message_received (struct cld_message *message, void *data)
{
	struct cld_service *service = data;
	printf("service %p received some message\n", service);
	return 1;
}

/** Creates a new service within the given daemon, communicating through fd. */
struct cld_service *
cld_service_create (struct cld_daemon *daemon, int fd)
{
	struct cld_service *service;
	
	service = malloc(sizeof *service);
	if (service == NULL)
		return NULL;
	
	service->daemon = daemon;
	service->fd = fd;
	
	service->connection = cld_connection_create(fd, message_received, service);
	if (service->connection == NULL) {
		free(service);
		return NULL;
	}
	
	service->source = cld_event_loop_add_fd(daemon->loop, fd, CLD_EVENT_READABLE, socket_data, service);
	if (service->source == NULL) {
		cld_connection_destroy(service->connection);
		free(service);
		return NULL;
	}
	
	printf("service connected %p\n", service);
	
	return service;
}

/** Closes the connection, removes the socket from the event loop and frees the
 * memory occupied by the service. */
void
cld_service_destroy (struct cld_service *service)
{
	printf("service disconnected %p\n", service);
	cld_connection_destroy(service->connection);
	cld_event_source_remove(service->source);
	free(service);
}
