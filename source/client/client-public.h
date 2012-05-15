/*
 * Copyright © 2012 Fabian Schuiki
 */

struct cld_client;
struct cld_object;
struct cld_account;

struct cld_client *cld_client_create();
void cld_client_destroy(struct cld_client *client);

struct cld_account *cld_client_add_account(struct cld_client *client, const char *type);
struct cld_account *cld_client_get_account(struct cld_client *client, const char *id);


//deprecated
void cld_client_account_set(struct cld_client *client, struct cld_object *account);
struct cld_object *cld_client_account_list(struct cld_client *client);
