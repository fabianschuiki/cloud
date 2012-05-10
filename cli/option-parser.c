/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "option-parser.h"


static void
handle_option (const struct cloud_option *option, char *value)
{
	switch (option->type) {
		case CLOUD_OPTION_INT:
			*(int *)option->data = strtol(value, NULL, 0);
			return;
		case CLOUD_OPTION_UINT:
			*(unsigned int *)option->data = strtoul(value, NULL, 0);
			return;
		case CLOUD_OPTION_STRING:
			*(char **)option->data = strdup(value);
			return;
		case CLOUD_OPTION_BOOL:
			*(int *)option->data = 1;
		default:
			fprintf(stderr, "command line option type %d not supported\n", option->type);
			assert(0);
	}
}

/** Looks for the given options in the given command line arguments, handles
 * them and removes them from the list of arguments. */
int
cloud_options_parse (const struct cloud_option *options, int count, int argc, char *argv[])
{
	int i, j, k, len = 0;
	
	for (i = 1, j = 1; i < argc; i++) {
		for (k = 0; k < count; k++) {
			if (options[k].name)
				len = strlen(options[k].name);
			if (argv[i][0] == '-') {
				if (options[k].name &&
					argv[i][1] == '-' &&
					strncmp(options[k].name, &argv[i][2], len) == 0 &&
					(argv[i][len + 2] == '=' || argv[i][len + 2] == 0)) {
					handle_option(&options[k], &argv[i][len + 3]);
					break;
				}
				else if (options[k].short_name && options[k].short_name == argv[i][1]) {
					handle_option(&options[k], &argv[i][2]);
					break;
				}
			}
			if (k == count)
				argv[j++] = argv[i];
		}
	}
	argv[j] = NULL;
	
	return j;
}