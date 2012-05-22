#include <stdio.h>
#include <cloud.h>


int main(int argc, char *argv[])
{
	printf("connecting to daemon\n");
	struct cld_client *client = cld_client_create();
	if (client == NULL)
		return -1;
	
	sleep(3);
	
	cld_client_destroy(client);
	return 0;
}