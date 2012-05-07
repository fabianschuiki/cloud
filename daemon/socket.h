/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef SOCKET_H
#define SOCKET_H


typedef enum {
	CLD_SOCKET_CLIENT,
	CLD_SOCKET_SERVICE
} cld_socket_type_t;

struct cld_socket;

struct cld_socket *cld_socket_create(cld_socket_type_t type);
void cld_socket_destroy(struct cld_socket *socket);

int cld_socket_listen(struct cld_socket *socket);
int cld_socket_connect(struct cld_socket *socket);

#endif