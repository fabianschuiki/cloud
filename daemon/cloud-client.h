/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_CLIENT_H
#define CLOUD_CLIENT_H


struct cld_client;

struct cld_client *cld_client_create();
void cld_client_destroy(struct cld_client *client);


#endif