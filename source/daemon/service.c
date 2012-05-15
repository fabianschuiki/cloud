/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "service.h"
#include "daemon.h"
#include "../event-loop.h"
#include "../connection.h"
#include "../object.h"


/** Called by the event loop whenever there is traffic on the service's socket.
 * Routes the traffic through to the service's connection. May destroy the
 * service if a disconnect is detected. */
static int
socket_data (int fd, int mask, void *data)
{
	struct cld_service *service = data;
	
	int len = cld_connection_communicate(service->connection, mask);
	if (len < 0) {
		cld_service_destroy(service);
		return 0;
	}
	
	return len;
}

/** Called by the service's connection whenever a message is received. */
static int
connection_received (struct cld_object *object, void *data)
{
	struct cld_service *service = data;
	printf("service %p sent ", service);
	cld_object_print(object);
	cld_object_destroy(object);
	
	/*switch (op) {
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
	}*/
	
	return 1;
}

static void
connection_disconnected (void *data)
{
	struct cld_service *service = data;
	cld_daemon_disconnect_service(service->daemon, service);
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
	
	service->connection = cld_connection_create(fd, connection_received, connection_disconnected, service);
	if (service->connection == NULL) {
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
	free(service);
}
