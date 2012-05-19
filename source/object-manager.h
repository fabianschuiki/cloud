/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H


struct cld_object_manager;

struct cld_object_manager *cld_object_manager_create ();
void cld_object_manager_destroy (struct cld_object_manager *manager);

void cld_object_manager_add (struct cld_object_manager *manager, struct cld_object *object);
void cld_object_manager_remove (struct cld_object_manager *manager, int id);

int cld_object_manager_handle (struct cld_object_manager *manager, struct cld_message *msg);


#endif