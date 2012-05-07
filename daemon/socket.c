/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stddef.h>

#include "socket.h"


struct cld_socket {
	int fd;
	struct sockaddr_un addr;
	size_t addr_size;
	struct cld_event_source *source;
};

struct cld_socket *
cld_socket_create (cld_socket_type_t type)
{
	struct cld_socket *socket;
	
	socket = malloc(sizeof *socket);
	if (socket == NULL)
		return NULL;
	
	socket->type = type;
	
	socket->fd = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (socket->fd < 0) {
		error("socket");
		free(socket);
		return NULL;
	}
	
	const char * name = NULL;
	switch (socket->type) {
		case CLD_SOCKET_CLIENT:  name = "/tmp/cloud.sock"; break;
		case CLD_SOCKET_SERVICE: name = "/tmp/cloud.service.sock"; break;
	}
	if (name == NULL) {
		fprintf(stderr, "%s: socket type %d not supported\n", __FUNCTION__, type);
		close(socket->fd);
		free(socket);
		return NULL;
	}
	
	memset(socket->addr, 0, sizeof socket->addr);
	socket->addr.sun_family = AF_LOCAL;
	size_t name_size = strcpy(socket->addr.sun_path, name);
	socket->addr_size = offsetof(struct sockaddr_un, sun_path) + name_size;
	
	return socket;
}

void
cld_socket_destroy (struct cld_socket* socket)
{
	close(socket->fd);
	unlink(socket->addr.sun_path);
	free(socket);
}


int
cld_socket_listen (struct cld_socket *socket)
{
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
	if (connect(socket->fd, (struct sockaddr *) &socket->addr, socket->addr_size) < 0) {
		error("connect");
		close(socket->fd);
		return -1;
	}
	
	return 0;
}
