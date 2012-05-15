/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "buffer.h"


struct cld_buffer *
cld_buffer_create ()
{
	struct cld_buffer *buffer;
	
	buffer = malloc(sizeof *buffer);
	if (buffer == NULL)
		return NULL;
	
	buffer->length = 0;
	buffer->size = 1024;
	buffer->data = malloc(buffer->size);
	
	return buffer;
}

void
cld_buffer_destroy (struct cld_buffer *buffer)
{
	free(buffer->data);
	free(buffer);
}

void
cld_buffer_resize (struct cld_buffer *buffer, size_t size)
{
	void *data = malloc(size);
	memcpy(data, buffer->data, buffer->size);
	free(buffer->data);
	buffer->data = data;
}

void
cld_buffer_extend (struct cld_buffer *buffer, size_t size)
{
	size_t new_size = buffer->size;
	while (new_size < buffer->length + size) new_size <<= 1;
	
	if (new_size != buffer->size)
		cld_buffer_resize(buffer, new_size);
}

void
cld_buffer_put (struct cld_buffer *buffer, const void *data, size_t length)
{
	cld_buffer_extend(buffer, length);
	memcpy(buffer->data + buffer->length, data, length);
	buffer->length += length;
}

void *
cld_buffer_slice (struct cld_buffer *buffer, size_t offset)
{
	void *data = buffer->data;
	buffer->data = malloc(buffer->size);
	buffer->length -= offset;
	assert(buffer->length >= 0 && "sliced beyond the end of the buffer");
	if (buffer->length > 0)
		memcpy(buffer->data, data + offset, buffer->length);
	return data;
}
