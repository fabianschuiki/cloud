/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H


typedef int (*cloud_command_func_t) (int argc, char *argv[]);

struct cloud_command {
	const char *name;
	cloud_command_func_t func;
};

int cloud_command_parse (struct cloud_command *commands, int count, int argc, char *argv[]);


#endif