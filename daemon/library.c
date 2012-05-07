#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <unistd.h>

#include "util.h"
#include "library.h"
#include "private.h"


struct cld_daemon {
	struct cld_connection *connection;
	int fd;
};


static int connection_update (struct cld_connection *connection, int mask, void *data)
{
	struct cld_daemon *daemon = data;
	
	//Do something useful with the connection update here...
	
	return 0;
}


struct cld_daemon *
cld_daemon_connect()
{
	struct cld_daemon *daemon;
	
	daemon = malloc(sizeof *daemon);
	if (daemon == NULL)
		return NULL;
	
	daemon->fd = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (daemon->fd < 0) {
		free(daemon);
		return NULL;
	}
	
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof addr);
	addr.sun_family = AF_LOCAL;
	socklen_t name_size = snprintf(addr.sun_path, sizeof addr.sun_path, "%s/%s", "/tmp", "cloud.socket") + 1;
	
	socklen_t size = offsetof(struct sockaddr_un, sun_path) + name_size;
	if (connect(daemon->fd, (struct sockaddr *) &addr, size) < 0) {
		error("connect");
		close(daemon->fd);
		free(daemon);
		return NULL;
	}
	
	daemon->connection = cld_connection_create(daemon->fd, connection_update, daemon);
	if (daemon->connection == NULL) {
		close(daemon->fd);
		free(daemon);
		return NULL;
	}
	
	return daemon;
}

void
cld_daemon_disconnect(struct cld_daemon *daemon)
{
	cld_connection_destroy(daemon->connection);
	close(daemon->fd);
	free(daemon);
}


void
cld_daemon_get_services (struct cld_daemon *daemon)
{
	cld_connection_write(daemon->connection, "services", 8);
	int len = cld_connection_data(daemon->connection, CLD_CONNECTION_READABLE);
	printf("len = %d\n", len);
}
