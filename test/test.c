#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "library.h"


int main(int argc, char *argv[])
{
	/*struct sockaddr_un addr;
	
	int fd = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (fd < 0)
		return -1;
	
	memset(&addr, 0, sizeof addr);
	addr.sun_family = AF_LOCAL;
	socklen_t name_size = snprintf(addr.sun_path, sizeof addr.sun_path, "%s/%s", "/tmp", "cloud.socket") + 1;
	
	socklen_t size = offsetof(struct sockaddr_un, sun_path) + name_size;
	if (connect(fd, (struct sockaddr *) &addr, size) < 0) {
		fprintf(stderr, "unable to connect to %s, errno: %d\n", addr.sun_path, errno);
		close(fd);
		return -1;
	}
	
	printf("waiting for data...\n");
	char buffer[1024];
	size_t s = read(fd, buffer, 1024);
	buffer[s] = 0;
	printf("read %s\n", buffer);*/
	
	struct cld_daemon *d;
	d = cld_daemon_connect();
	if (d == NULL)
		return -1;
	
	printf("connected to daemon\n");
	
	//Fetch a list of available services.
	cld_daemon_get_services(d);
	
	cld_daemon_disconnect(d);
	
	return 0;
}

