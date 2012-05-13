/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <string.h>

#include "list.h"


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


void
cld_list_add (struct cld_list *list, void *object)
{
	struct cld_list_element *element = cld_list_element_create(object);
	if (element == NULL)
		return;
	
	element->prev = list->last;
	if (list->last)
		list->last->next = element;
	if (list->first == NULL)
		list->first = element;
	list->last = element;
}

void
cld_list_remove (struct cld_list *list, void *object)
{
	struct cld_list_element *element = list->first;
	for (; element && element->object != object; element = element->next);
	if (element == NULL)
		return;
	
	if (element->prev) element->prev->next = element->next;
	if (element->next) element->next->prev = element->prev;
	if (list->first == element) list->first = element->next;
	if (list->last  == element) list->last  = element->prev;
}


int
cld_list_count (struct cld_list *list)
{
	struct cld_list_element *element = list->first;
	int c = 0;
	for (; element; element = element->next, c++);
	return c;
}