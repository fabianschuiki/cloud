/*
 * Copyright © 2012 Fabian Schuiki
 */

struct cld_object;
struct cld_object_manager;


struct cld_object *cld_object_create_object (struct cld_object_manager *manager, const char *type);
struct cld_object *cld_object_create_string (struct cld_object_manager *manager, const char *string);
struct cld_object *cld_object_create_array (struct cld_object_manager *manager);
void cld_object_destroy (struct cld_object *object);

void cld_object_ref (struct cld_object *object);
void cld_object_unref (struct cld_object *object);

int cld_object_is_object (struct cld_object *object, const char *type);
int cld_object_is_string (struct cld_object *object);
int cld_object_is_array (struct cld_object *object);

void cld_object_set (struct cld_object *object, const char *name, struct cld_object *value);
struct cld_object *cld_object_get (struct cld_object *object, const char *name);
struct cld_object *cld_object_at (struct cld_object *object, int index);

int cld_object_add (struct cld_object *object, struct cld_object *value);
void cld_object_remove (struct cld_object *object, int index);
void cld_object_remove_object (struct cld_object *object, struct cld_object *value);
struct cld_object *cld_object_at (struct cld_object *object, int index);
int cld_object_count (struct cld_object *object);

int cld_object_find (struct cld_object *object, struct cld_object *value);


//UNREVIEWED DEPRECATED STUFF
void cld_object_print(struct cld_object *object);