/*
 * Copyright © 2012 Fabian Schuiki
 */

struct cld_service;

struct cld_service *cld_service_create();
void cld_service_destroy(struct cld_service *service);

int cld_service_run(struct cld_service *service);

void cld_service_set_name(struct cld_service *service, const char *name);
