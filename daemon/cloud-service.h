/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_SERVICE_H
#define CLOUD_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif


struct cld_service;

struct cld_service *cld_service_create();
void cld_service_destroy(struct cld_service *service);


#ifdef __cplusplus
}
#endif

#endif
