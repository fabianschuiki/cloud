/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "account-public.h"


struct cld_object;

typedef void (*cld_account_commit_func_t)(struct cld_account *account, void *data);

struct cld_account *cld_account_create(struct cld_object *object, cld_account_commit_func_t func, void *data);
void cld_account_destroy(struct cld_account *account);

void cld_account_commit(struct cld_account *account);


#endif