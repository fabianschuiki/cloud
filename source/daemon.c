/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <assert.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/signalfd.h>
#include <sys/select.h>
#include <signal.h>

#include "util.h"
#include "daemon.h"
#include "socket.h"
#include "daemon/client.h"
#include "daemon/service.h"
#include "run-loop.h"
#include "fd-public.h"
#include "list.h"
#include "connection.h"


struct cld_daemon *
cld_daemon_create ()
{
	struct cld_daemon *daemon;
	
	daemon = malloc(sizeof *daemon);
	if (daemon == NULL)
		return NULL;
	
	memset(daemon, 0, sizeof *daemon);
	
	daemon->client_socket = cld_socket_create(CLD_SOCKET_CLIENT);
	if (daemon->client_socket == NULL) {
		free(daemon);
		return NULL;
	}
	if (cld_socket_listen(daemon->client_socket) < 0) {
		cld_socket_destroy(daemon->client_socket);
		free(daemon);
		return NULL;
	}
	
	daemon->service_socket = cld_socket_create(CLD_SOCKET_SERVICE);
	if (daemon->service_socket == NULL) {
		cld_socket_destroy(daemon->client_socket);
		free(daemon);
		return NULL;
	}
	if (cld_socket_listen(daemon->service_socket) < 0) {
		cld_socket_destroy(daemon->client_socket);
		cld_socket_destroy(daemon->service_socket);
		free(daemon);
		return NULL;
	}
	
	daemon->clients = cld_list_create();
	daemon->services = cld_list_create();
	daemon->connections = cld_list_create();
	
	return daemon;
}

void
cld_daemon_destroy (struct cld_daemon *daemon)
{
	cld_socket_destroy(daemon->client_socket);
	cld_socket_destroy(daemon->service_socket);
	
	cld_list_destroy(daemon->connections);
	
	struct cld_list_element *client = cld_list_begin(daemon->clients);
	for (; client; client = cld_list_next(client)) {
		cld_client_destroy(client->object);
	}
	cld_list_destroy(daemon->clients);
	
	struct cld_list_element *service = cld_list_begin(daemon->services);
	for (; service; service = cld_list_next(service)) {
		cld_client_destroy(service->object);
	}
	cld_list_destroy(daemon->services);
	
	free(daemon);
}


void
cld_daemon_disconnect_client (struct cld_daemon *daemon, struct cld_client *client)
{
	cld_list_remove(daemon->clients, client);
	cld_list_remove(daemon->connections, client->connection);
	cld_client_destroy(client);
}

void
cld_daemon_disconnect_service (struct cld_daemon *daemon, struct cld_service *service)
{
	cld_list_remove(daemon->services, service);
	cld_list_remove(daemon->connections, service->connection);
	cld_service_destroy(service);
}


static void
accept_connection (struct cld_daemon *daemon, struct cld_socket *socket)
{
	printf("accepting connection\n");
	
	struct sockaddr_un name;
	socklen_t length = sizeof name;
	int fd = accept(cld_socket_get_fd(socket), (struct sockaddr *) &name, &length);
	if (fd < 0) {
		error("accept()");
		return;
	}
	
	if (socket == daemon->client_socket) {
		struct cld_client *client = cld_client_create(daemon, fd);
		cld_list_add(daemon->clients, client);
		cld_list_add(daemon->connections, client->connection);
	}
	else if (socket == daemon->service_socket) {
		struct cld_service *service = cld_service_create(daemon, fd);
		cld_list_add(daemon->services, service);
		cld_list_add(daemon->connections, service->connection);
	}
	else {
		assert(0 && "socket not part of daemon");
	}
}


/** Called by the run loop. Returns a list of file descriptors in use by the
 * daemon that need to be monitored. Returns the client and service listening
 * sockets, as well as the sockets of connected clients and services. */
struct cld_fd *
runloop_fd (int *count, void *data)
{
	struct cld_daemon *daemon = data;
	
	*count = 2 + cld_list_count(daemon->connections);
	
	struct cld_fd *fds = malloc(*count * sizeof(*fds));
	if (fds == NULL)
		return NULL;
	
	fds[0].fd = cld_socket_get_fd(daemon->client_socket);
	fds[0].mask = CLD_FD_READ;
	fds[1].fd = cld_socket_get_fd(daemon->service_socket);
	fds[1].mask = CLD_FD_READ;
	
	int i = 2;
	struct cld_list_element *element = cld_list_begin(daemon->connections);
	for (; element; element = cld_list_next(element)) {
		struct cld_connection *connection = element->object;
		fds[i].fd = connection->fd;
		fds[i].mask = connection->mask;
		i++;
	}
	
	return fds;
}

/** Called by the run loop whenever a file descriptor is ready for reading or
 * writing. */
void
runloop_activity (struct cld_fd *fds, int num_fds, void *data)
{
	struct cld_daemon *daemon = data;
	
	int i;
	for (i = 0; i < num_fds; i++) {
		if (fds[i].fd == cld_socket_get_fd(daemon->client_socket))
			accept_connection(daemon, daemon->client_socket);
		else if (fds[i].fd == cld_socket_get_fd(daemon->service_socket))
			accept_connection(daemon, daemon->service_socket);
		else {
			struct cld_list_element *element = cld_list_begin(daemon->connections);
			while (element) {
				struct cld_connection *connection = element->object;
				element = cld_list_next(element);
				if (connection->fd == fds[i].fd)
					cld_connection_communicate(connection, fds[i].mask);
			}
		}
	}
}


int main(int argc, char* argv[])
{
	struct cld_daemon * daemon;
	
	daemon = cld_daemon_create();
	if (daemon == NULL)
		return 1;
	
	struct cld_runloop *loop = cld_runloop_create();
	if (loop == NULL) {
		fprintf(stderr, "unable to create runloop\n");
		return 1;
	}
	
	cld_runloop_callback_fd(loop, runloop_fd, daemon);
	cld_runloop_callback_activity(loop, runloop_activity, daemon);
	
	int retval = cld_runloop_run(loop);
	
	printf("terminating\n");
	cld_daemon_destroy(daemon);
	
	return retval;
}
