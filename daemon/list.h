/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef LIST_H
#define LIST_H


struct cld_list;
struct cld_list_element;

struct cld_list *cld_list_create();
void cld_list_destroy(struct cld_list *list);

struct cld_list_element *cld_list_begin(struct cld_list *list);
struct cld_list_element *cld_list_next(struct cld_list_element *element);


#endif