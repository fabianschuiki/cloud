/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CONNECTION_H
#define CONNECTION_H


struct cld_connection;
struct cld_message {
	int op;
	int length;
};

typedef int (*cld_connection_message_func_t) (struct cld_message *message, void *data);
typedef int (*cld_connection_update_func_t) (struct cld_connection *connection, int mask, void *data);

struct cld_connection *cld_connection_create (int fd, cld_connection_message_func_t message, void *data);
void cld_connection_destroy (struct cld_connection *connection);

int cld_connection_data (struct cld_connection *connection, int mask);
int cld_connection_write (struct cld_connection *connection, const void *data, size_t length);


#endif