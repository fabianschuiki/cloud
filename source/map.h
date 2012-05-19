/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef MAP_H
#define MAP_H


struct cld_map;
struct cld_map_element;


struct cld_map *cld_map_create ();
void cld_map_destroy (struct cld_map *map);


#endif