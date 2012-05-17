/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_DAEMON_H
#define CLOUD_DAEMON_H


struct cld_socket;
struct cld_list;

struct cld_client;
struct cld_service;
struct cld_object;

struct cld_daemon {
	struct cld_socket *client_socket;
	struct cld_socket *service_socket;
	
	struct cld_list *clients;
	struct cld_list *services;
	struct cld_list *connections;
	
	struct cld_object *accounts;
};

void cld_daemon_disconnect_client(struct cld_daemon *daemon, struct cld_client *client);
void cld_daemon_disconnect_service(struct cld_daemon *daemon, struct cld_service *service);

struct cld_object *cld_daemon_add_account (struct cld_daemon *daemon, const char *type);
int cld_daemon_update_account (struct cld_daemon *daemon, struct cld_object *account);

void cld_daemon_accounts_save (struct cld_daemon *daemon);
void cld_daemon_accounts_load (struct cld_daemon *daemon);


#endif