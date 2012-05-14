/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H


struct cloud_command;

typedef int (*cloud_command_func_t) (struct cloud_command *command, int argc, char *argv[]);

struct cloud_command {
	const char *name;
	const char *usage;
	cloud_command_func_t func;
	const char *info;
};

int cloud_command_parse (struct cloud_command *commands, int count, int argc, char *argv[]);


#endif