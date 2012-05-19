/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <string.h>

#include "map.h"


struct cld_map {
	
};

struct cld_map *
cld_map_create ()
{
	struct cld_map *map;
	
	map = malloc(sizeof *map);
	if (map == NULL)
		return NULL;
	
	memset(map, 0, sizeof *map);
	
	return map;
}

void
cld_map_destroy (struct cld_map *map)
{
	free(map);
}