/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_H
#define CLOUD_H

#ifdef __cplusplus
extern "C" {
#endif


struct cld_daemon;

struct cld_daemon *cld_daemon_connect();
void cld_daemon_disconnect(struct cld_daemon *daemon);

void cld_daemon_get_services(struct cld_daemon *daemon);


#ifdef __cplusplus
}
#endif

#endif