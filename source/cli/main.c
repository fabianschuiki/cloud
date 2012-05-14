/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdio.h>
#include <string.h>
#include <cloud.h>
//#include "command-parser.h"


struct cld_client *cloud = NULL;

int cloud_cmd_account(int argc, char *argv[]);

static void
print_usage ()
{
	printf(
		"usage: cloud [--version] [-h|--help] <command> [<args>]\n"
	);
}

static void
print_help ()
{
	print_usage();
	printf("\n"
		"The following commands are supported:\n"
		"   account    Manage the cloud accounts.\n"
	);
}

/*int
cloud_cmd_root (struct cloud_command *cmd, int argc, char *argv[])
{
	return 0;
}*/

int main(int argc, char *argv[])
{
	/*struct cloud_command root = {
		"cloud", "[--version] <command> [<args>]",
		cloud_cmd_root,
		NULL
	};
	
	return cloud_command_parse(&root, 1, argc, argv);*/
	
	if (argc < 2) {
		print_help();
		return 0;
	}
	
	const char *command = argv[1];
	if (strcmp(command, "-h") == 0) {
		print_usage();
		return 0;
	}
	if (strcmp(command, "--help") == 0) {
		print_help();
		return 0;
	}
	if (strcmp(command, "--version") == 0) {
		printf("cloud version %d.%d.%d\n", 0, 1, 0);
		return 0;
	}
	
	cloud = cld_client_create();
	if (cloud == NULL)
		return -1;
	
	argc -= 2;
	argv = &argv[2];
	if (strcmp(command, "account") == 0) {
		return cloud_cmd_account(argc, argv);
	}
	
	fprintf(stderr, "Unknown command '%s'. Try cloud --help.\n", command);
	return 1;
}