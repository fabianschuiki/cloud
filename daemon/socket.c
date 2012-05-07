/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stddef.h>

#include "util.h"
#include "socket.h"


struct cld_socket {
	int fd;
	struct sockaddr_un addr;
	size_t addr_size;
	int listen;
};

struct cld_socket *
cld_socket_create (cld_socket_type_t type)
{
	struct cld_socket *sock;
	
	sock = malloc(sizeof *sock);
	if (sock == NULL)
		return NULL;
	
	sock->listen = 0;
	
	sock->fd = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (sock->fd < 0) {
		error("socket");
		free(sock);
		return NULL;
	}
	
	const char * name = NULL;
	switch (type) {
		case CLD_SOCKET_CLIENT:  name = "/tmp/cloud.sock"; break;
		case CLD_SOCKET_SERVICE: name = "/tmp/cloud.service.sock"; break;
	}
	if (name == NULL) {
		fprintf(stderr, "%s: socket type %d not supported\n", __FUNCTION__, type);
		close(sock->fd);
		free(sock);
		return NULL;
	}
	
	memset(&sock->addr, 0, sizeof sock->addr);
	sock->addr.sun_family = AF_LOCAL;
	strcpy(sock->addr.sun_path, name);
	sock->addr_size = offsetof(struct sockaddr_un, sun_path) + strlen(sock->addr.sun_path) + 1;
	
	return sock;
}

void
cld_socket_destroy (struct cld_socket* socket)
{
	close(socket->fd);
	if (socket->listen)
		unlink(socket->addr.sun_path);
	free(socket);
}


int
cld_socket_listen (struct cld_socket *socket)
{
	printf("listening on %s\n", socket->addr.sun_path);
	socket->listen = 1;
	
	if (bind(socket->fd, (struct sockaddr *) &socket->addr, socket->addr_size) < 0) {
		error("bind");
		close(socket->fd);
		return -1;
	}
	
	if (listen(socket->fd, 1) < 0) {
		error("listen");
		close(socket->fd);
		unlink(socket->addr.sun_path);
		return -1;
	}
	
	return 0;
}

int
cld_socket_connect (struct cld_socket *socket)
{
	printf("connecting to %s\n", socket->addr.sun_path);
	
	if (connect(socket->fd, (struct sockaddr *) &socket->addr, socket->addr_size) < 0) {
		error("connect");
		close(socket->fd);
		return -1;
	}
	
	return 0;
}

int
cld_socket_get_fd (struct cld_socket *socket)
{
	return socket->fd;
}
