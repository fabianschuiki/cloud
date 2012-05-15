/*
 * Copyright © 2012 Fabian Schuiki
 */

struct cld_account;

void cld_account_set (struct cld_account *account, const char *field, const char *value);
const char *cld_account_get (struct cld_account *account, const char *field);

const char *cld_account_get_id (struct cld_account *account);

void cld_account_set_identity (struct cld_account *account, const char *ident);
const char *cld_account_get_identity (struct cld_account *account);