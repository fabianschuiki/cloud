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


//UNREVIEWED DEPRECATED STUFF
void cld_object_set(struct cld_object *object, const char *name, struct cld_object *value);
struct cld_object *cld_object_get(struct cld_object *object, const char *name);
const char *cld_object_get_string(struct cld_object *object, const char *name);

const char *cld_object_string (struct cld_object *object);

void cld_object_array_add(struct cld_object *object, struct cld_object *value);
void cld_object_array_remove(struct cld_object *object, unsigned int index);
void cld_object_array_remove_object(struct cld_object *object, struct cld_object *value);
void cld_object_array_set(struct cld_object *object, unsigned int index, struct cld_object *value);
struct cld_object *cld_object_array_get(struct cld_object *object, unsigned int index);
unsigned int cld_object_array_count(struct cld_object *object);
int cld_object_array_find(struct cld_object *object, struct cld_object *value);

void cld_object_print(struct cld_object *object);