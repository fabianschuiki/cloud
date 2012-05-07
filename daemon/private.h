/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_PRIVATE_H
#define CLOUD_PRIVATE_H

#include <stdint.h>


enum {
	CLD_CONNECTION_READABLE = (1 << 0),
	CLD_CONNECTION_WRITABLE = (1 << 1)
};

struct cld_connection;
struct cld_connection_message {
	uint16_t op;
	uint16_t length;
};

typedef int (*cld_connection_message_func_t) (struct cld_connection_message *message);
typedef int (*cld_connection_update_func_t) (struct cld_connection *connection, int mask, void *data);

struct cld_connection *cld_connection_create (int fd, cld_connection_update_func_t update, void *data);
void cld_connection_destroy (struct cld_connection *connection);

int cld_connection_data (struct cld_connection *connection, int mask);
int cld_connection_write (struct cld_connection *connection, const void *data, size_t length);


#endif
