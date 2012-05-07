/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_SERVICE_PRIVATE_H
#define CLOUD_SERVICE_PRIVATE_H

#include "cloud-service.h"


struct cld_service {
	int run;
	struct cld_daemon *daemon;
	struct cld_event_loop *loop;
	
	const char *name;
};


#endif