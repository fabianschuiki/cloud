/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "service.h"
#include "../cloud-daemon.h"
#include "../event-loop.h"
#include "../connection.h"
#include "../protocol.h"


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
message_received (int op, void *message, size_t length, void *data)
{
	struct cld_service *service = data;
	printf("service %p received message %d of %d bytes\n", service, op, (int)length);
	
	switch (op) {
		case CLD_OP_SERVICE_RECORD: {
			struct cld_service_record *record = message;
			void *ptr = message + sizeof *record;
			
			if (service->name) free(service->name);
			service->name = malloc(record->name_len + 1);
			if (service->name == NULL)
				return -1;
			
			memcpy(service->name, ptr, record->name_len);
			service->name[record->name_len] = 0;
			ptr += record->name_len;
			
			printf("service %p is called %s\n", service, service->name);
		} break;
		
		default:
			fprintf(stderr, "service %p sent unknown opcode %d\n", service, op);
			return -1;
	}
	
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
	
	memset(service, 0, sizeof *service);
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
