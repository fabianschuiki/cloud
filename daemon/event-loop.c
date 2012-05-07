/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/signalfd.h>

#include "cloud-util.h"
#include "cloud-daemon.h"


struct cld_event_source_interface {
	int (*dispatch) (struct cld_event_source *source, struct epoll_event *ep);
	int (*remove) (struct cld_event_source *source);
};

struct cld_event_loop {
	int epoll_fd;
};

struct cld_event_source {
	struct cld_event_source_interface *interface;
	struct cld_event_loop *loop;
	void *data;
	int fd;
};


struct cld_event_source_signal {
	struct cld_event_source base;
	int signal_number;
	cld_event_loop_signal_func_t func;
};

static int
cld_event_source_signal_dispatch(struct cld_event_source *source,
	struct epoll_event *ep)
{
	struct cld_event_source_signal * signal_source = (struct cld_event_source_signal *) source;
	
	struct signalfd_siginfo signal_info;
	int len = read(source->fd, &signal_info, sizeof signal_info);
	if (len != sizeof signal_info) {
		error("signalfd read");
		return -1;
	}
	
	return signal_source->func(signal_source->signal_number, signal_source->base.data);
}

static int
cld_event_source_signal_remove(struct cld_event_source *source)
{
	close(source->fd);
	return 0;
}

struct cld_event_source_interface signal_source_interface = {
	cld_event_source_signal_dispatch,
	cld_event_source_signal_remove
};

struct cld_event_source *
cld_event_loop_add_signal (struct cld_event_loop *loop,
	int signal_number,
	cld_event_loop_signal_func_t func,
	void *data)
{
	struct cld_event_source_signal *source;
	source = malloc(sizeof *source);
	if (source == NULL)
		return NULL;
	
	//Initialize the source.
	source->base.interface = &signal_source_interface;
	source->base.loop = loop;
	source->base.data = data;
	source->signal_number = signal_number;
	source->func = func;
	
	//Open the signal file descriptor.
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, signal_number);
	source->base.fd = signalfd(-1, &mask, SFD_CLOEXEC);
	if (source->base.fd < 0) {
		error("signalfd");
		free(source);
		return NULL;
	}
	sigprocmask(SIG_BLOCK, &mask, NULL);
	
	//Setup the epoll event for this source.
	struct epoll_event ep;
	memset(&ep, 0, sizeof ep);
	ep.events = EPOLLIN;
	ep.data.ptr = source;
	
	//Register the event.
	if (epoll_ctl(loop->epoll_fd, EPOLL_CTL_ADD, source->base.fd, &ep) < 0) {
		error("epoll_ctl");
		close(source->base.fd);
		free(source);
		return NULL;
	}
	
	return &source->base;
}


struct cld_event_loop *
cld_event_loop_create ()
{
	struct cld_event_loop *loop;
	
	loop = malloc(sizeof *loop);
	if (loop == NULL)
		return NULL;
	
	loop->epoll_fd = epoll_create(1);
	if (loop->epoll_fd < 0) {
		error("epoll_create");
		free(loop);
		return NULL;
	}
	
	return loop;
}

void
cld_event_loop_destroy (struct cld_event_loop *loop)
{
	close(loop->epoll_fd);
	free(loop);
}

/**
 * Blocks until one or more events are received, then dispatches them.
 * */
int
cld_event_loop_dispatch (struct cld_event_loop *loop)
{
	//Wait for events.
	const int num_ep = 32;
	struct epoll_event ep[num_ep];
	int count = epoll_wait(loop->epoll_fd, ep, num_ep, 1000);
	if (count < 0) {
		error("epoll_wait");
		return -1;
	}
	
	//Dispatch events.
	int i;
	for (i = 0; i < count; i++) {
		struct cld_event_source *source = ep[i].data.ptr;
		if (source->fd != -1)
			source->interface->dispatch(source, &ep[i]);
	}
	
	return 0;
}
