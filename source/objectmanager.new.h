/*
 * Copyright © 2012 Fabian Schuiki
 */

struct cld_object_manager {
	struct cld_map *objects;
	struct cld_map *local;
};

struct cld_object_manager *cld_object_manager_create ();
void cld_object_manager_destroy (struct cld_object_manager *manager);

//Objects created locally
void cld_object_manager_add (struct cld_object_manager *manager);
void cld_object_manager_remove (struct cld_object_manager *manager, int id);

/** Create, update or destroy objects. */
int cld_object_manager_handle (struct cld_object_manager *manager, struct cld_message *msg);