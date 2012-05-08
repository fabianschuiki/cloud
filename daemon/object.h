/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>


struct cld_object;
struct cld_buffer;

struct cld_buffer *cld_object_serialize(struct cld_object *object);
struct cld_object *cld_object_unserialize(void *buffer, size_t length);


#endif
