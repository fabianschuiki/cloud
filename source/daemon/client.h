/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef DAEMON_CLIENT_H
#define DAEMON_CLIENT_H


struct cld_daemon;

struct cld_client {
	struct cld_daemon *daemon;
	int fd;
	struct cld_connection *connection;
};

struct cld_client *cld_client_create(struct cld_daemon *daemon, int fd);
void cld_client_destroy(struct cld_client *client);


#endif