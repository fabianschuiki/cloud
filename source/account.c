/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <string.h>

#include "account.h"
#include "object.h"


struct cld_account {
	struct cld_object *object;
	
	cld_account_commit_func_t commit;
	void *data;
};


struct cld_account *
cld_account_create (struct cld_object *object, cld_account_commit_func_t func, void *data)
{
	struct cld_account *account;
	
	account = malloc(sizeof *account);
	if (account == NULL)
		return NULL;
	
	memset(account, 0, sizeof *account);
	account->object = object;
	account->commit = func;
	account->data = data;
	
	return account;
}

void
cld_account_destroy (struct cld_account *account)
{
	cld_object_destroy(account->object);
	free(account);
}


struct cld_object *
cld_account_get_object (struct cld_account *account)
{
	return account->object;
}


void
cld_account_commit (struct cld_account *account)
{
	account->commit(account, account->data);
}


void
cld_account_set (struct cld_account *account, const char *field, const char *value)
{
	cld_object_set(account->object, field, cld_object_create_string(value));
	cld_account_commit(account);
}

const char *
cld_account_get (struct cld_account *account, const char *field)
{
	return cld_object_get_string(account->object, field);
}


const char *
cld_account_get_id (struct cld_account *account)
{
	return cld_account_get(account, "uuid");
}


void
cld_account_set_identity (struct cld_account *account, const char *ident)
{
	cld_account_set(account, "identity", ident);
}

const char *
cld_account_get_identity (struct cld_account *account)
{
	return cld_account_get(account, "identity");
}