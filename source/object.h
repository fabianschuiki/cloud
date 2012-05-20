/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include "object-public.h"


enum {
	CLD_TYPE_STRING,
	CLD_TYPE_ARRAY,
	CLD_TYPE_OBJECT
};

struct cld_object_field {
	char *name;
	struct cld_object *object;
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
			struct cld_object_field *fields;
			int num_fields;
			int max_fields;
		};
	};
};

struct cld_object *cld_object_create (struct cld_object_manager *manager);

unsigned char cld_object_get_kind (struct cld_object *object);
unsigned int cld_object_get_id (struct cld_object *object);

void cld_object_fields_resize (struct cld_object *object, int max);
void cld_object_fields_extend (struct cld_object *object, int num);

struct cld_object_field *cld_object_add_field (struct cld_object *object);
struct cld_object_field *cld_object_find_field (struct cld_object *object, const char *name);
struct cld_object_field *cld_object_get_field (struct cld_object *object, const char *name);

// Serialization
struct cld_buffer;
struct cld_buffer *cld_object_serialize(struct cld_object *object);
struct cld_object *cld_object_unserialize(void *buffer, size_t length);


#endif
