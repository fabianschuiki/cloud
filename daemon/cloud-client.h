/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_H
#define CLOUD_H

#ifdef __cplusplus
extern "C" {
#endif


struct cld_client;

struct cld_client *cld_client_create();
void cld_client_destroy(struct cld_client *client);


#ifdef __cplusplus
}
#endif

#endif