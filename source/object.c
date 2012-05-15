/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "object.h"
#include "buffer.h"


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
	
	if (name != NULL) {
		field->name = malloc(strlen(name) + 1);
		if (field->name == NULL)
			return NULL;
		strcpy(field->name, name);
	}
	
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



static const char * const CLD_OBJECT_STRING_TYPE = "@s";
static const char * const CLD_OBJECT_ARRAY_TYPE = "@a";

struct cld_object {
	char *type;
	union {
		char *str;
		struct {
			struct cld_field *field_head;
			struct cld_field *field_tail;
		};
	};
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
	
	object->str = malloc(strlen(string) + 1);
	strcpy(object->str, string);
	
	return object;
}

struct cld_object *
cld_object_create_array ()
{
	return cld_object_create(CLD_OBJECT_ARRAY_TYPE);
}

void
cld_object_destroy (struct cld_object *object)
{
	if (cld_object_is_string(object)) {
		free(object->str);
	}
	
	else {
		struct cld_field *field = object->field_head;
		while (field) {
			struct cld_field *tmp = field;
			field = field->next;
			cld_field_destroy(tmp);
		}
	}
	
	if (object->type)
		free(object->type);
	free(object);
}


static void
add_field (struct cld_object *object, struct cld_field *field)
{
	assert(field->next == NULL && field->prev == NULL);
	
	field->prev = object->field_tail;
	if (object->field_tail)
		object->field_tail->next = field;
	object->field_tail = field;
	
	if (object->field_head == NULL)
		object->field_head = field;
}

static void
remove_field (struct cld_object *object, struct cld_field *field)
{
	if (field->next) field->next->prev = field->prev;
	if (field->prev) field->prev->next = field->next;
	if (object->field_head == field) object->field_head = field->next;
	if (object->field_tail == field) object->field_tail = field->prev;
}


/** Sets the value of the object's field with the given name. Overwrites what-
 * ever is in the field at the moment. By adding an object the caller hands off
 * responsibility to free the object; field values are automatically freed
 * whenever they're overwritten or the parent object gets destroyed. */
void
cld_object_set (struct cld_object *object, const char *name, struct cld_object *value)
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

/** Returns the value of the object's field with the given name. Returns NULL
 * if the field does not exist. Note that if the field is set to NULL the
 * function will also return NULL. */
struct cld_object *
cld_object_get (struct cld_object *object, const char *name)
{
	struct cld_field *field = object->field_head;
	while (field) {
		if (strcmp(field->name, name) == 0)
			return field->object;
		field = field->next;
	}
	return NULL;
}

/** Convenience function that returns the object's field with the given name
 * as a string. */
const char *
cld_object_get_string (struct cld_object *object, const char *name)
{
	struct cld_object *string = cld_object_get(object, name);
	if (string == NULL || !cld_object_is_string(string))
		return NULL;
	return cld_object_string(string);
}

/** Returns the object's string value. */
const char *
cld_object_string (struct cld_object *object)
{
	return object->str;
}

static struct cld_field *
get_array_field (struct cld_object *object, unsigned int index)
{
	struct cld_field *field = object->field_head;
	int i = 0;
	while (field && i++ < index) {
		field = field->next;
	}
	if (field == NULL) {
		fprintf(stderr, "%s: index %i out of bounds\n", __FUNCTION__, index);
		return NULL;
	}
	return field;
}

void
cld_object_array_add (struct cld_object *object, struct cld_object *value)
{
	struct cld_field *field = cld_field_create(NULL);
	if (field == NULL)
		return;
	add_field(object, field);
	cld_field_set_object(field, value);
}

/** Removes the given index from the array. */
void
cld_object_array_remove (struct cld_object *object, unsigned int index)
{
	struct cld_field *field = get_array_field(object, index);
	if (field == NULL)
		return;
	remove_field(object, field);
	cld_field_destroy(field);
}

/** Removes the given object from the array. */
void
cld_object_array_remove_object (struct cld_object *object, struct cld_object *value)
{
	int index = cld_object_array_find(object, value);
	if (index < 0)
		return;
	cld_object_array_remove(object, index);
}

void
cld_object_array_set (struct cld_object *object, unsigned int index, struct cld_object *value)
{
	struct cld_field *field = get_array_field(object, index);
	if (field == NULL)
		return;
	cld_field_set_object(field, value);
}

struct cld_object *
cld_object_array_get (struct cld_object *object, unsigned int index)
{
	struct cld_field *field = get_array_field(object, index);
	if (field == NULL)
		return NULL;
	return field->object;
}

unsigned int
cld_object_array_count (struct cld_object *object)
{
	unsigned int c = 0;
	struct cld_field *field = object->field_head;
	while (field) {
		c++;
		field = field->next;
	}
	return c;
}

/** Returns the index of the value in the given array, or -1 if it is not part
 * of the array. */
int
cld_object_array_find (struct cld_object *object, struct cld_object *value)
{
	struct cld_field *field = object->field_head;
	int i = 0;
	while (field && field->object != value) {
		i++;
		field = field->next;
	}
	if (field == NULL)
		return -1;
	return i;
}


static void
print (struct cld_object *object, int indent)
{
	char in[128];
	int i;
	for (i = 0; i < indent * 4 && i < 127; i++)
		in[i] = ' ';
	in[i] = 0;
	
	//Null objects
	if (object == NULL) {
		printf("NULL");
	}
	
	//Strings
	else if (cld_object_is_string(object)) {
		printf("\"%s\"", object->str);
		return;
	}
	
	//Arrays
	else if (cld_object_is_array(object)) {
		printf("[");
		struct cld_field *field = object->field_head;
		int i = 0;
		while (field) {
			printf("\n%s    %i = ", in, i++);
			print(field->object, indent + 1);
			field = field->next;
		}
		printf("\n%s]", in);
	}
	
	//Objects
	else {
		printf("%s {", object->type);
		struct cld_field *field = object->field_head;
		while (field) {
			printf("\n%s    %s = ", in, field->name);
			print(field->object, indent + 1);
			field = field->next;
		}
		printf("\n%s}", in);
	}
}

/** Dumps the object to standard output. Nice for debugging. */
void
cld_object_print (struct cld_object *object)
{
	print(object, 0);
	printf("\n");
}

int
cld_object_is (struct cld_object *object, const char *type)
{
	return strcmp(object->type, type) == 0;
}

int cld_object_is_string (struct cld_object *object)
{
	return cld_object_is(object, CLD_OBJECT_STRING_TYPE);
}

int cld_object_is_array (struct cld_object *object)
{
	return cld_object_is(object, CLD_OBJECT_ARRAY_TYPE);
}


/* Serializes the given object into the buffer. The object may be NULL. */
static int
serialize (struct cld_object *object, struct cld_buffer *buffer)
{
	int length = 0;
	int offset = buffer->length;
	cld_buffer_put(buffer, &length, sizeof length);
	
	if (object) {
		cld_buffer_put(buffer, object->type, strlen(object->type) + 1);
		
		//Strings
		if (cld_object_is_string(object)) {
			cld_buffer_put(buffer, object->str, strlen(object->str) + 1);
		}
		
		//Arrays
		else if (cld_object_is_array(object)) {
			struct cld_field *field = object->field_head;
			while (field) {
				serialize(field->object, buffer);
				field = field->next;
			}
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
	
	//Unserialize Strings
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
	
	//Unserialize Arrays
	else if (strcmp(type, CLD_OBJECT_ARRAY_TYPE) == 0) {
		struct cld_object *object = cld_object_create_array();
		if (object == NULL)
			return NULL;
		
		int i = 0;
		while (*data - base < obj_length) {
			struct cld_object *value = unserialize(data, length - (*data - base));
			if (value == NULL) {
				fprintf(stderr, "%s: unable to unserialize array element %i\n", __FUNCTION__, i);
				cld_object_destroy(object);
				return NULL;
			}
			i++;
			cld_object_array_add(object, value);
		}
		
		return object;
	}
	
	//Unserialize Objects
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
			
			cld_object_set(object, name, value);
		}
		
		return object;
	}
}

/** Reconstructs the object from the given bytes. */
struct cld_object *
cld_object_unserialize (void *buffer, size_t length)
{
	const void *data = buffer;
	return unserialize(&data, length);
}