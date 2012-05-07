/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "cloud-service-private.h"
#include "socket.h"
#include "event-loop.h"
#include "service/daemon.h"


struct cld_service *
cld_service_create ()
{
	struct cld_service *service;
	
	service = malloc(sizeof *service);
	if (service == NULL)
		return NULL;
	
	service->run = 1;
	
	service->loop = cld_event_loop_create();
	if (service->loop == NULL) {
		free(service);
		return NULL;
	}
	
	service->daemon = cld_daemon_connect(service);
	if (service->daemon == NULL) {
		cld_event_loop_destroy(service->loop);
		free(service);
		return NULL;
	}
	
	return service;
}

void
cld_service_destroy (struct cld_service *service)
{
	cld_daemon_disconnect(service->daemon);
	cld_event_loop_destroy(service->loop);
	free(service);
}

static int
signal_terminate (int signal_number, void *data)
{
	struct cld_service *service = data;
	service->run = 0;
}

int
cld_service_run (struct cld_service *service)
{
	cld_event_loop_add_signal(service->loop, SIGTERM, signal_terminate, service);
	cld_event_loop_add_signal(service->loop, SIGQUIT, signal_terminate, service);
	cld_event_loop_add_signal(service->loop, SIGINT, signal_terminate, service);
	
	while (service->run) {
		if (cld_event_loop_dispatch(service->loop) < 0)
			return -1;
	}
	
	printf("gracefully terminating service\n");
	
	return 0;
}

void
cld_service_set_name (struct cld_service *service, const char *name)
{
	service->name = name;
	//TODO: inform daemon of updated name.
}
