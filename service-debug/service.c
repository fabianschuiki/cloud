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
	
	//Do something useful here.
	sleep(3);
	
	cld_service_destroy(service);
	return 0;
}
