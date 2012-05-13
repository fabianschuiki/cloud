/*
 * Copyright © 2012 Fabian Schuiki
 */

struct cld_service;
struct cld_fd;

struct cld_service *cld_service_create();
void cld_service_destroy(struct cld_service *service);

struct cld_fd *cld_service_get_fds(struct cld_service *service, int *count);
void cld_service_communicate(struct cld_service *service, struct cld_fd *fds, int count);

void cld_service_set_name(struct cld_service *service, const char *name);
void cld_service_send_descriptor (struct cld_service *service);