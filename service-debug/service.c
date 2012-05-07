/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdio.h>
#include <cloud-service.h>


int main(int argc, char *argv[])
{
	printf("launching cloud service debug\n");
	
	struct cld_service *service = cld_service_create();
	if (service == NULL) {
		fprintf(stderr, "unable to create cloud service\n");
		return -1;
	}
	
	//Configure the service.
	cld_service_set_name(service, "debug");
	
	int res = cld_service_run(service);
	cld_service_destroy(service);
	
	return res;
}
