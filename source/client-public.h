/*
 * Copyright © 2012 Fabian Schuiki
 */

struct cld_client;
struct cld_object;

struct cld_client *cld_client_create();
void cld_client_destroy(struct cld_client *client);

void cld_client_account_set(struct cld_client *client, struct cld_object *account);
struct cld_object *cld_client_account_list(struct cld_client *client);
