/*
 * Copyright © 2012 Fabian Schuiki
 */

struct cld_field {
	struct cld_field *next;
	struct cld_field *prev;
	unsigned int id;
	union {
		char *name;
		int index;
	};
};

enum {
	CLD_STRING,
	CLD_ARRAY,
	CLD_OBJECT
};

struct cld_object {
	unsigned int id;
	struct cld_object_manager *manager;
	int refcount;
	
	unsigned char kind;
	union {
		char *string;
		struct {
			char *type;
			struct cld_field *field_head;
			struct cld_field *field_last;
		};
	};
};

struct cld_object *cld_object_create (struct cld_object_manager *manager);
void cld_object_destroy (struct cld_object *object);

struct cld_object *cld_object_create_object (struct cld_object_manager *manager, const char *type);
struct cld_object *cld_object_create_string (struct cld_object_manager *manager, const char *string);
struct cld_object *cld_object_create_array (struct cld_object_manager *manager);

unsigned char cld_object_get_kind (struct cld_object *object);
unsigned int cld_object_get_id (struct cld_object *object);

void cld_object_ref (struct cld_object *object);
void cld_object_unref (struct cld_object *object);