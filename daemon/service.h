/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef SERVICE_H
#define SERVICE_H

#include "service-public.h"


struct cld_service {
	struct cld_socket *socket;
	struct cld_connection *connection;
	
	const char *name;
};


#endif