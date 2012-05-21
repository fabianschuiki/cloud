/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H


struct cld_object;
struct cld_object_manager;
struct cld_message;

typedef int (*cld_object_manager_send_func_t) (struct cld_message *msg, void *data);


struct cld_object_manager *cld_object_manager_create ();
void cld_object_manager_destroy (struct cld_object_manager *manager);

void cld_object_manager_add (struct cld_object_manager *manager, struct cld_object *object);
void cld_object_manager_remove (struct cld_object_manager *manager, int id);

int cld_object_manager_handle (struct cld_object_manager *manager, struct cld_message *msg);

void cld_object_manager_send_func (struct cld_object_manager *manager, cld_object_manager_send_func_t func, void *data);
void cld_object_manager_send (struct cld_object_manager *manager, struct cld_message *message);


#endif