/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <string.h>

#include "list.h"


struct cld_list_element {
	struct cld_list_element *next;
	struct cld_list_element *prev;
	void *object;
};

struct cld_list_element *
cld_list_element_create (void *object)
{
	struct cld_list_element *element;
	
	element = malloc(sizeof *element);
	if (element == NULL)
		return NULL;
	
	memset(element, 0, sizeof *element);
	element->object = object;
	
	return element;
}

void
cld_list_element_destroy (struct cld_list_element *element)
{
	free(element);
}


struct cld_list {
	struct cld_list_element *first;
	struct cld_list_element *last;
};

struct cld_list *
cld_list_create ()
{
	struct cld_list *list;
	
	list = malloc(sizeof *list);
	if (list == NULL)
		return NULL;
	
	memset(list, 0, sizeof *list);
	
	return list;
}

void
cld_list_destroy (struct cld_list *list)
{
	struct cld_list_element *element = list->first;
	for (; element; element = element->next)
		cld_list_element_destroy(element);
	free(list);
}


struct cld_list_element *
cld_list_begin (struct cld_list *list)
{
	return list->first;
}

struct cld_list_element *
cld_list_next (struct cld_list_element *element)
{
	return element->next;
}