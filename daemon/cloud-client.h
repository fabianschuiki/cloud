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


struct cld_object;

struct cld_object *cld_object_create(const char *type);
struct cld_object *cld_object_create_string(const char *string);
void cld_object_destroy(struct cld_object *object);

void cld_object_set_field(struct cld_object *object, const char *name, struct cld_object *value);
struct cld_object *cld_object_get_field(struct cld_object *object, const char *name);

void cld_object_print(struct cld_object *object);


#ifdef __cplusplus
}
#endif

#endif