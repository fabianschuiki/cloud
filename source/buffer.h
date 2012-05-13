/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>


struct cld_buffer {
	void *data;
	int length;
	size_t size;
};

struct cld_buffer *cld_buffer_create();
void cld_buffer_destroy(struct cld_buffer *buffer);

void cld_buffer_resize(struct cld_buffer *buffer, size_t size);
void cld_buffer_extend(struct cld_buffer *buffer, size_t size);

void cld_buffer_put(struct cld_buffer *buffer, const void *data, size_t length);
void *cld_buffer_slice (struct cld_buffer *buffer, size_t offset);


#endif