/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_CLIENT_PRIVATE_H
#define CLOUD_CLIENT_PRIVATE_H

#include "client-public.h"


struct cld_client {
	struct cld_socket *socket;
	struct cld_connection *connection;
	
	struct cld_list *accounts;
};


#endif