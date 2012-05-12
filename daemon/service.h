/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef SERVICE_H
#define SERVICE_H

#include "service-public.h"


struct cld_service {
	int run;
	struct cld_daemon *daemon;
	//struct cld_event_loop *loop;
	
	const char *name;
};


#endif