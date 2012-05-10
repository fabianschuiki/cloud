/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLIENT_DAEMON_H
#define CLIENT_DAEMON_H


struct cld_daemon {
	struct cld_client *client;
	struct cld_socket *socket;
	struct cld_event_source *source;
	struct cld_connection *connection;
};

struct cld_daemon *cld_daemon_connect();
void cld_daemon_disconnect(struct cld_daemon *daemon);


#endif