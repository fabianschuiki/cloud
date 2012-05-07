/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#include "util.h"
#include "cloud-daemon.h"
#include "private.h"
#include "socket.h"


struct cld_daemon {
	struct cld_socket *client_socket;
	struct cld_socket *service_socket;
	struct cld_event_loop *loop;
	int run;
};


struct cld_daemon *
cld_daemon_create ()
{
	struct cld_daemon *d;
	
	d = malloc(sizeof *d);
	if (d == NULL)
		return NULL;
	
	memset(d, 0, sizeof *d);
	d->run = 1;
	
	d->loop = cld_event_loop_create();
	if (d->loop == NULL) {
		free(d);
		return NULL;
	}
	
	return d;
}

void
cld_daemon_destroy (struct cld_daemon *d)
{
	cld_event_loop_destroy(d->loop);
	cld_socket_destroy(d->client_socket);
	cld_socket_destroy(d->service_socket);
	free(d);
}

/*int
socket_data (int fd, int mask, void *data)
{
	struct cld_daemon * daemon = data;
	
	struct sockaddr_un name;
	socklen_t length = sizeof name;
	int client_fd = accept(fd, (struct sockaddr *) &name, &length);
	if (client_fd < 0) {
		error("accept");
	} else
		cld_client_create(daemon, client_fd);
	
	return 1;
}*/

int
cld_daemon_socket_client_open (struct cld_daemon *daemon)
{
	daemon->client_socket = cld_socket_create(CLD_SOCKET_CLIENT);
	if (daemon->client_socket == NULL)
		return -1;
	
	if (cld_socket_listen(daemon->client_socket) < 0) {
		cld_socket_destroy(daemon->client_socket);
		daemon->client_socket = NULL;
		return -1;
	}
	
	return 0;
}

int
cld_daemon_socket_service_open (struct cld_daemon *daemon)
{
	daemon->service_socket = cld_socket_create(CLD_SOCKET_SERVICE);
	if (daemon->service_socket == NULL)
		return -1;
	
	if (cld_socket_listen(daemon->service_socket) < 0) {
		cld_socket_destroy(daemon->service_socket);
		daemon->service_socket = NULL;
		return -1;
	}
	
	return 0;
}

struct cld_event_loop *
cld_daemon_get_event_loop (struct cld_daemon *daemon)
{
	return daemon->loop;
}


struct cld_client {
	struct cld_daemon *daemon;
	struct cld_connection *connection;
	struct cld_event_source *source;
};


int
client_connection_data (int fd, int mask, void *data)
{
	struct cld_client *client = data;
	struct cld_connection *connection = client->connection;
	
	int cmask = 0;
	if (mask & CLD_EVENT_READABLE)
		cmask |= CLD_CONNECTION_READABLE;
	if (mask & CLD_EVENT_WRITABLE)
		cmask |= CLD_CONNECTION_WRITABLE;
	
	int len = cld_connection_data(connection, cmask);
	if (len < 0) {
		//cld_client_destroy(client);
		return 1;
	}
	
	return 1;
}

int
client_connection_update (struct cld_connection *connection, int mask, void *data)
{
	struct cld_client *client = data;
	
	int emask = 0;
	if (mask & CLD_CONNECTION_READABLE)
		emask |= CLD_EVENT_READABLE;
	if (mask & CLD_CONNECTION_WRITABLE)
		emask |= CLD_EVENT_WRITABLE;
	
	return cld_event_source_fd_update(client->source, emask);
}

/*struct cld_client *
cld_client_create (struct cld_daemon *daemon, int fd)
{
	struct cld_client *client;
	
	client = malloc(sizeof *client);
	if (client == NULL)
		return NULL;
	
	memset(client, 0, sizeof *client);
	client->daemon = daemon;
	
	client->connection = cld_connection_create(fd, cld_client_connection_update, client);
	if (client->connection == NULL) {
		free(client);
		return NULL;
	}
	
	client->source = cld_event_loop_add_fd(cld_daemon_get_event_loop(daemon), fd, CLD_EVENT_READABLE, cld_client_connection_data, client);
	if (client->source == NULL) {
		free(client);
		return NULL;
	}
	
	printf("connected to client %p\n", client);
	
	return client;
}

void cld_client_destroy (struct cld_client *client)
{
	printf("disconnect from client %p\n", client);
	
	cld_event_source_remove(client->source);
	free(client);
}*/


int
signal_quit (int signal_number, void *data)
{
	struct cld_daemon *d = data;
	d->run = 0;
}


int main(int argc, char* argv[])
{
	struct cld_daemon * daemon;
	
	daemon = cld_daemon_create();
	if (daemon == NULL)
		return -1;
	
	if (cld_daemon_socket_client_open(daemon) < 0)
		return -1;
	if (cld_daemon_socket_service_open(daemon) < 0)
		return -1;
	
	cld_event_loop_add_signal(daemon->loop, SIGTERM, signal_quit, daemon);
	cld_event_loop_add_signal(daemon->loop, SIGQUIT, signal_quit, daemon);
	cld_event_loop_add_signal(daemon->loop, SIGINT, signal_quit, daemon);
	
	while (daemon->run) {
		if (cld_event_loop_dispatch(daemon->loop) < 0)
			return -1;
	}
	
	printf("terminating cloud daemon\n");
	
	cld_daemon_destroy(daemon);
	
	return 0;
}
