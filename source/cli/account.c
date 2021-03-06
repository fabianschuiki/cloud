/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cloud.h>

#include "main.h"


static void
print_usage ()
{
	printf(
		"usage: cloud account <command> [<args>]\n"
	);
}

static void
print_help ()
{
	print_usage();
	printf("\n"
		"The following commands are supported:\n"
		"   add     Add a new account of the given type\n"
		"   list    Show a list of cloud accounts\n"
		"   set     Sets the value of an account field\n"
	);
}

int
cloud_cmd_account_add (int argc, char *argv[])
{
	if (argc < 1) {
		fprintf(stderr, "account type required\n");
		return -1;
	}
	
	/*struct cld_object *account = cld_object_create("account");
	if (account == NULL)
		return -1;
	
	cld_object_set(account, "type", cld_object_create_string(argv[0]));
	cld_client_account_set(cloud, account);
	cld_object_destroy(account);*/
	
	struct cld_account *account = cld_client_add_account(cloud, argv[0]);
	if (account == NULL) {
		fprintf(stderr, "unable to create account\n");
		return -1;
	}
	
	printf("%s\n", cld_account_get_id(account));
	
	return 0;
}

int
cloud_cmd_account_list (int argc, char *argv[])
{
	struct cld_object *accounts = cld_client_account_list(cloud);
	if (accounts == NULL)
		return -1;
	
	int num = cld_object_array_count(accounts);
	int i;
	for (i = 0; i < num; i++) {
		struct cld_object *account = cld_object_array_get(accounts, i);
		printf("%s  %s  %s\n", cld_object_get_string(account, "uuid"), cld_object_get_string(account, "type"), cld_object_get_string(account, "identity"));
	}
	
	cld_object_destroy(accounts);
	return 0;
}

int
cloud_cmd_account_set (int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "account ID and at least one field=value pair required\n");
		return -1;
	}
	const char *uuid = argv[0];
	
	struct cld_account *account = cld_client_get_account(cloud, uuid);
	if (account == NULL) {
		fprintf(stderr, "unknown account %s\n", uuid);
		return -1;
	}
	
	int i;
	for (i = 1; i < argc; i++) {
		char *field = argv[i];
		char *value = strchr(field, '=');
		value[0] = 0;
		value++;
		cld_account_set(account, field, value);
	}
	
	return 0;
}

int
cloud_cmd_account (int argc, char *argv[])
{
	if (argc < 1) {
		print_help();
		return 0;
	}
	
	const char *command = argv[0];
	if (strcmp(command, "-h") == 0) {
		print_usage();
		return 0;
	}
	if (strcmp(command, "--help") == 0) {
		print_help();
		return 0;
	}
	
	argc--;
	argv = &argv[1];
	if (strcmp(command, "add") == 0) {
		return cloud_cmd_account_add(argc, argv);
	}
	if (strcmp(command, "list") == 0) {
		return cloud_cmd_account_list(argc, argv);
	}
	if (strcmp(command, "set") == 0) {
		return cloud_cmd_account_set(argc, argv);
	}
	
	return 0;
}