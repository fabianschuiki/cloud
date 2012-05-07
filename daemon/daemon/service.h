/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef DAEMON_SERVICE_H
#define DAEMON_SERVICE_H


struct cld_daemon;

struct cld_service {
	struct cld_daemon *daemon;
	int fd;
	struct cld_event_source *source;
	struct cld_connection *connection;
	char *name;
};

struct cld_service *cld_service_create(struct cld_daemon *daemon, int fd);
void cld_service_destroy(struct cld_service *service);


#endif