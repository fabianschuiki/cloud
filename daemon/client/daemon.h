/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLIENT_DAEMON_H
#define CLIENT_DAEMON_H


struct cld_daemon;

struct cld_daemon *cld_daemon_connect();
void cld_daemon_disconnect(struct cld_daemon *daemon);


#endif