/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>


struct cld_object;
struct cld_connection;

typedef int (*cld_connection_received_func_t) (struct cld_object *object, void *data);

struct cld_connection *cld_connection_create (int fd, cld_connection_received_func_t received, void *data);
void cld_connection_destroy (struct cld_connection *connection);

int cld_connection_data (struct cld_connection *connection, int mask);
int cld_connection_write (struct cld_connection *connection, struct cld_object *object);


#endif