/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_DAEMON_H
#define CLOUD_DAEMON_H


struct cld_daemon {
	struct cld_socket *client_socket;
	struct cld_socket *service_socket;
	
	struct cld_event_source *client_source;
	struct cld_event_source *service_source;
	
	struct cld_event_loop *loop;
	int run;
};


#endif