/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cloud-client.h"
#include "object.h"
#include "buffer.h"


static const char * const CLD_OBJECT_STRING_TYPE = "@s";

struct cld_field {
	struct cld_field *next;
	struct cld_field *prev;
	char *name;
	struct cld_object *object;
};

struct cld_field *
cld_field_create (const char *name)
{
	struct cld_field *field;
	
	field = malloc(sizeof *field);
	if (field == NULL)
		return NULL;
	
	memset(field, 0, sizeof *field);
	
	field->name = malloc(strlen(name) + 1);
	if (field->name == NULL)
		return NULL;
	strcpy(field->name, name);
	
	return field;
}

void
cld_field_destroy (struct cld_field *field)
{
	if (field->name)
		free(field->name);
	cld_object_destroy(field->object);
	free(field);
}

void
cld_field_set_object (struct cld_field *field, struct cld_object *object)
{
	if (field->object)
		cld_object_destroy(field->object);
	field->object = object;
}


struct cld_object {
	char *type;
	void *value;
	struct cld_field *field_head;
	struct cld_field *field_tail;
};


struct cld_object *
cld_object_create (const char *type)
{
	struct cld_object *object;
	
	object = malloc(sizeof *object);
	if (object == NULL)
		return NULL;
	
	memset(object, 0, sizeof *object);
	
	object->type = malloc(strlen(type) + 1);
	if (object->type == NULL)
		return NULL;
	strcpy(object->type, type);
	
	return object;
}

struct cld_object *
cld_object_create_string (const char *string)
{
	struct cld_object *object;
	
	object = cld_object_create(CLD_OBJECT_STRING_TYPE);
	if (object == NULL)
		return NULL;
	
	object->value = malloc(strlen(string) + 1);
	strcpy(object->value, string);
	
	return object;
}

void
cld_object_destroy (struct cld_object *object)
{
	struct cld_field *field = object->field_head;
	while (field) {
		struct cld_field *tmp = field;
		field = field->next;
		cld_field_destroy(tmp);
	}
	
	if (object->type)
		free(object->type);
	if (object->value)
		free(object->value);
	free(object);
}


static void
add_field (struct cld_object *object, struct cld_field *field)
{
	assert(field->next == NULL && field->prev == NULL);
	
	field->prev = object->field_tail;
	if (object->field_tail)
		object->field_tail->next = field;
	else
		object->field_tail = field;
	
	if (object->field_head == NULL)
		object->field_head = field;
}


void
cld_object_set_field (struct cld_object *object, const char *name, struct cld_object *value)
{
	struct cld_field *field = object->field_head;
	while (field && strcmp(field->name, name) != 0)
		field = field->next;
	
	if (field == NULL) {
		field = cld_field_create(name);
		if (field == NULL)
			return;
		add_field(object, field);
	}
	
	cld_field_set_object(field, value);
}

struct cld_object *
cld_object_get_field (struct cld_object *object, const char *name)
{
	struct cld_field *field = object->field_head;
	while (field) {
		if (strcmp(field->name, name) == 0)
			return field->object;
		field = field->next;
	}
	return NULL;
}


static void
print (struct cld_object *object, int indent)
{
	if (object == NULL) {
		printf("NULL");
		return;
	}
	
	if (strcmp(object->type, CLD_OBJECT_STRING_TYPE) == 0) {
		printf("\"%s\"", (char *)object->value);
		return;
	}
	
	char in[128];
	int i;
	for (i = 0; i < indent * 4; i++)
		in[i] = ' ';
	in[i] = 0;
	
	printf("%s {", object->type);
	
	struct cld_field *field = object->field_head;
	while (field) {
		printf("\n %s   %s = ", in, field->name);
		print(field->object, indent + 1);
		field = field->next;
	}
	
	printf("\n%s}", in);
}

void
cld_object_print (struct cld_object *object)
{
	print(object, 0);
	printf("\n");
}


static int
serialize (struct cld_object *object, struct cld_buffer *buffer)
{
	int length = 0;
	int offset = buffer->length;
	cld_buffer_put(buffer, &length, sizeof length);
	
	if (object) {
		cld_buffer_put(buffer, object->type, strlen(object->type) + 1);
		
		//Strings
		if (strcmp(object->type, CLD_OBJECT_STRING_TYPE) == 0) {
			cld_buffer_put(buffer, object->value, strlen(object->value) + 1);
		}
		
		//Objects
		else {
			struct cld_field *field = object->field_head;
			while (field) {
				cld_buffer_put(buffer, field->name, strlen(field->name) + 1);
				serialize(field->object, buffer);
				field = field->next;
			}
		}
	}
	
	//Fix the length of the object.
	length = buffer->length - offset;
	*(int *)(buffer->data + offset) = length;
	
	return 0;
}

/** Serializes the object and returns a pointer to a newly allocated region of
 * memory containing the object. The caller is responsible for freeing the re-
 * turned memory after usage. */
struct cld_buffer *
cld_object_serialize (struct cld_object *object)
{
	struct cld_buffer *buffer = cld_buffer_create();
	if (buffer == NULL)
		return NULL;
	
	if (serialize(object, buffer) < 0) {
		cld_buffer_destroy(buffer);
		return NULL;
	}
	
	return buffer;
}


static struct cld_object *
unserialize (const void **data, size_t length)
{
	const void *base = *data;
	
	if (length < sizeof(int)) {
		fprintf(stderr, "%s: unexpected end of data\n", __FUNCTION__);
		return NULL;
	}
	
	int obj_length = *(int *)*data;
	*data += sizeof(int);
	if (obj_length > length) {
		fprintf(stderr, "%s: object of %u bytes, but only %lu bytes left in the buffer\n", __FUNCTION__, obj_length, length);
		return NULL;
	}
	
	const char *type = *data;
	for (; *(char *)*data != 0; (*data)++) {
		if (*data - base >= length) {
			fprintf(stderr, "%s: unexpected end of data when reading object type\n", __FUNCTION__);
			return NULL;
		}
	}
	(*data)++;
	
	if (strcmp(type, CLD_OBJECT_STRING_TYPE) == 0) {
		const char *value = *data;
		for (; *(char *)*data != 0; (*data)++) {
			if (*data - base >= length) {
				fprintf(stderr, "%s: unexpected end of data when reading string\n", __FUNCTION__);
				return NULL;
			}
		}
		(*data)++;
		
		return cld_object_create_string(value);
	}
	else {
		struct cld_object *object = cld_object_create(type);
		if (object == NULL)
			return NULL;
		
		while (*data - base < obj_length) {
			const char *name = *data;
			for (; *(char *)*data != 0; (*data)++) {
				if (*data - base >= length) {
					fprintf(stderr, "%s: unexpected end of data when reading field name in object %s\n", __FUNCTION__, type);
					cld_object_destroy(object);
					return NULL;
				}
			}
			(*data)++;
			
			struct cld_object *value = unserialize(data, length - (*data - base));
			if (value == NULL) {
				fprintf(stderr, "%s: unable to unserialize field %s\n", __FUNCTION__, name);
				cld_object_destroy(object);
				return NULL;
			}
			
			cld_object_set_field(object, name, value);
		}
		
		return object;
	}
}

/** Reconstructs the object from the given bytes. */
struct cld_object *
cld_object_unserialize (struct cld_buffer *buffer)
{
	const void *data = buffer->data;
	return unserialize(&data, buffer->length);
}