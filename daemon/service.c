/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "service.h"
#include "socket.h"
#include "connection.h"
#include "service/daemon.h"
#include "fd-public.h"


struct cld_service *
cld_service_create ()
{
	struct cld_service *service;
	
	service = malloc(sizeof *service);
	if (service == NULL)
		return NULL;
	
	service->run = 1;
	
	/*service->loop = cld_event_loop_create();
	if (service->loop == NULL) {
		free(service);
		return NULL;
	}*/
	
	service->daemon = cld_daemon_connect(service);
	if (service->daemon == NULL) {
		//cld_event_loop_destroy(service->loop);
		free(service);
		return NULL;
	}
	
	return service;
}

void
cld_service_destroy (struct cld_service *service)
{
	cld_daemon_disconnect(service->daemon);
	//cld_event_loop_destroy(service->loop);
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
	
	fds[0].fd = service->daemon->connection->fd;
	fds[0].mask = CLD_FD_READ;
	
	return fds;
}

void
cld_service_poll (struct cld_service *service)
{
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
	/*cld_event_loop_add_signal(service->loop, SIGTERM, signal_terminate, service);
	cld_event_loop_add_signal(service->loop, SIGQUIT, signal_terminate, service);
	cld_event_loop_add_signal(service->loop, SIGINT, signal_terminate, service);
	
	cld_daemon_send_service_record(service->daemon, service->name);
	
	while (service->run) {
		if (cld_event_loop_dispatch(service->loop) < 0)
			return -1;
	}
	
	printf("gracefully terminating service\n");*/
	
	printf("ignoring deprecated cld_service_run\n");
	return 0;
}

void
cld_service_set_name (struct cld_service *service, const char *name)
{
	service->name = name;
	//TODO: inform daemon of updated name.
}
