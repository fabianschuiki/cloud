/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#include "cloud-util.h"
#include "cloud-daemon.h"


struct cld_socket {
	int fd;
	struct sockaddr_un addr;
};

struct cld_daemon {
	struct cld_socket *socket;
	struct cld_event_loop *loop;
	int run;
};


struct cld_daemon *
cld_daemon_create ()
{
	struct cld_daemon *d;
	
	d = malloc(sizeof *d);
	if (d == NULL)
		return NULL;
	
	memset(d, 0, sizeof *d);
	d->run = 1;
	
	d->loop = cld_event_loop_create();
	if (d->loop == NULL) {
		free(d);
		return NULL;
	}
	
	return d;
}

void
cld_daemon_destroy (struct cld_daemon *d)
{
	cld_event_loop_destroy(d->loop);
	
	free(d);
}

int
cld_daemon_socket_open (struct cld_daemon *d)
{
	struct cld_socket *s;
	
	s = malloc(sizeof *s);
	if (s == NULL)
		return -1;
	
	s->fd = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (s->fd < 0) {
		free(s);
		return -1;
	}
	
	memset(&s->addr, 0, sizeof s->addr);
	s->addr.sun_family = AF_LOCAL;
	socklen_t name_size = snprintf(s->addr.sun_path, sizeof s->addr.sun_path, "%s/%s", "/tmp", "cloud.socket") + 1;
	printf("using socket %s\n", s->addr.sun_path);
	
	socklen_t size = offsetof(struct sockaddr_un, sun_path) + name_size;
	if (bind(s->fd, (struct sockaddr *) &s->addr, size) < 0) {
		error("bind");
		close(s->fd);
		free(s);
		return -1;
	}
	
	if (listen(s->fd, 1) < 0) {
		error("listen");
		close(s->fd);
		unlink(s->addr.sun_path);
		free(s);
		return -1;
	}
	
	d->socket = s;
	return 0;
}

void
cld_daemon_socket_close (struct cld_daemon *d)
{
	close(d->socket->fd);
	unlink(d->socket->addr.sun_path);
	free(d->socket);
	d->socket = NULL;
}


int
signal_quit (int signal_number, void *data)
{
	struct cld_daemon *d = data;
	d->run = 0;
}


int main(int argc, char* argv[])
{
	struct cld_daemon * d;
	
	d = cld_daemon_create();
	if (d == NULL)
		return -1;
	
	if (cld_daemon_socket_open(d) < 0)
		return -1;
	
	cld_event_loop_add_signal(d->loop, SIGTERM, signal_quit, d);
	cld_event_loop_add_signal(d->loop, SIGQUIT, signal_quit, d);
	cld_event_loop_add_signal(d->loop, SIGINT, signal_quit, d);
	
	while (d->run) {
		if (cld_event_loop_dispatch(d->loop) < 0)
			return -1;
	}
	
	/*printf("listening on socket %s\n", s->addr.sun_path);
	
	while (1) {
		struct sockaddr_un name;
		socklen_t length = sizeof(name);
		int client_fd = accept(s->fd, (struct sockaddr *) &name, &length);
		if (client_fd < 0)
			perror("accept");
		else {
			printf("accepted connection\n");
			write(client_fd, "Hello\n", 6);
			close(client_fd);
		}
	}*/
	
	printf("terminating cloud daemon\n");
	
	cld_daemon_socket_close(d);
	cld_daemon_destroy(d);
	
	return 0;
}
