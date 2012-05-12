/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_DAEMON_H
#define CLOUD_DAEMON_H


struct cld_socket;
struct cld_list;

struct cld_daemon {
	struct cld_socket *client_socket;
	struct cld_socket *service_socket;
	
	struct cld_list *clients;
	struct cld_list *services;
};


#endif