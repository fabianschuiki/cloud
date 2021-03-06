/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef SERVICE_DAEMON_H
#define SERVICE_DAEMON_H


struct cld_daemon {
	struct cld_service *service;
	struct cld_socket *socket;
	struct cld_connection *connection;
};

struct cld_daemon *cld_daemon_connect();
void cld_daemon_disconnect(struct cld_daemon *daemon);

int cld_daemon_send_service_record(struct cld_daemon *daemon, const char *name);


#endif