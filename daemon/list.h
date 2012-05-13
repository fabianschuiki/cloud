/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef LIST_H
#define LIST_H


struct cld_list;
struct cld_list_element {
	void *object;
	struct cld_list_element *next;
	struct cld_list_element *prev;
};

struct cld_list *cld_list_create();
void cld_list_destroy(struct cld_list *list);

struct cld_list_element *cld_list_begin(struct cld_list *list);
struct cld_list_element *cld_list_next(struct cld_list_element *element);

void cld_list_add(struct cld_list *list, void *object);
void cld_list_remove(struct cld_list *list, void *object);

int cld_list_count(struct cld_list *list);


#endif