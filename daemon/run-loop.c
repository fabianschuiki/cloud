/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include "run-loop.h"
#include "fd-public.h"
#include "util.h"


struct cld_runloop {
	int run;
	int timeout;
	
	cld_runloop_fd_func_t fd;
	cld_runloop_activity_func_t activity;
	cld_runloop_idle_func_t idle;
	
	void *fd_data;
	void *activity_data;
	void *idle_data;
	
	int sigfd;
};

/** Creates a new run loop instance. In order to use the loop, you have to at
 * least provide an fd callback. */
struct cld_runloop *
cld_runloop_create ()
{
	struct cld_runloop *loop;
	
	loop = malloc(sizeof *loop);
	if (loop == NULL)
		return NULL;
	
	memset(loop, 0, sizeof *loop);
	loop->run = 1;
	loop->timeout = -1;
	
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGTERM);
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGINT);
	loop->sigfd = signalfd(-1, &mask, 0);
	if (loop->sigfd < 0) {
		error("signalfd()");
		free(loop);
		return NULL;
	}
	sigprocmask(SIG_BLOCK, &mask, NULL);
	
	return loop;
}

void
cld_runloop_destroy (struct cld_runloop *loop)
{
	if (loop->sigfd)
		close(loop->sigfd);
	free(loop);
}


static void
catch_signal (struct cld_runloop *loop)
{
	struct signalfd_siginfo signal_info;
	
	ssize_t len = read(loop->sigfd, &signal_info, sizeof signal_info);
	if (len != sizeof signal_info) {
		error("read(signalfd)");
		return;
	}
	
	int sig = signal_info.ssi_signo;
	printf("caught signal %d\n", sig);
	
	loop->run = 0;
}


/** Sets the run loop's timeout, that is the period of time after which the
 * idle callback is called if there's no activity on any of the watched file
 * descriptors.
 * @param timeout Timeout in microseconds. If -1, the runloop will block
 *     infinitely and the idle callback is never called. */
void
cld_runloop_set_timeout (struct cld_runloop *loop, int timeout)
{
	loop->timeout = timeout;
}

/** Starts the run loop and blocks until stopped either by a signal or manually
 * terminating the run loop through cld_runloop_stop. */
int
cld_runloop_run (struct cld_runloop *loop)
{
	while (loop->run) {
		if (loop->fd == NULL) {
			fprintf(stderr, "%s: no fd callback provided, which is required\n", __FUNCTION__);
			return -1;
		}
		
		int num_fds = 0;
		struct cld_fd *fds = loop->fd(&num_fds, loop->fd_data);
		if (fds == NULL) {
			fprintf(stderr, "%s: fd callback returned NULL instead of list of fds to watch\n", __FUNCTION__);
			return -1;
		}
		
		fd_set rfds, wfds;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		
		FD_SET(loop->sigfd, &rfds);
		
		int max_fd = loop->sigfd;
		int i;
		for (i = 0; i < num_fds; i++) {
			if (fds[i].mask & CLD_FD_READ)  FD_SET(fds[i].fd, &rfds);
			if (fds[i].mask & CLD_FD_WRITE) FD_SET(fds[i].fd, &wfds);
			if (fds[i].fd > max_fd) max_fd = fds[i].fd;
		}
		
		struct timeval tv;
		if (loop->timeout >= 0) {
			tv.tv_sec = loop->timeout / 1000000;
			tv.tv_usec = loop->timeout % 1000000;
		}
		
		int changed = select(max_fd + 1, &rfds, &wfds, NULL, (loop->timeout >= 0 ? &tv : NULL));
		if (changed < 0) {
			error("select()");
			free(fds);
			return -1;
		}
		else if (changed) {
			struct cld_fd *cfds;
			
			cfds = malloc(changed * sizeof(*cfds));
			if (cfds == NULL)
				return;
			
			memset(cfds, 0, changed * sizeof(*cfds));
			
			int num_cfds = 0;
			for (i = 0; i < num_fds; i++) {
				int mask = 0;
				if (FD_ISSET(fds[i].fd, &rfds)) mask |= CLD_FD_READ;
				if (FD_ISSET(fds[i].fd, &wfds)) mask |= CLD_FD_WRITE;
				if (mask == 0) continue;
				
				struct cld_fd *fd = NULL;
				int n;
				for (n = 0; n < num_cfds && cfds[n].fd != fds[n].fd; n++);
				fd = &cfds[n];
				if (n >= num_cfds) num_cfds = n + 1;
				
				fd->fd = fds[i].fd;
				fd->mask = mask;
			}
			
			if (num_cfds > 0 && loop->activity)
				loop->activity(cfds, num_cfds, loop->activity_data);
			
			free(cfds);
			
			if (FD_ISSET(loop->sigfd, &rfds))
				catch_signal(loop);
		}
		else {
			if (loop->idle)
				loop->idle(loop->idle_data);
		}
		
		free(fds);
	}
	
	return 0;
}

/** Stops the run loop. This function merely marks the loop as to be terminated,
 * but returns immediately. */
void
cld_runloop_stop (struct cld_runloop *loop)
{
	loop->run = 0;
}


/** Sets the runloop's file descriptor callback function, which should return
 * the file descriptors to observe. */
void
cld_runloop_callback_fd (struct cld_runloop *loop, cld_runloop_fd_func_t func, void *data)
{
	loop->fd = func;
	loop->fd_data = data;
}

/** Sets the runloop's activity callback function, which is executed whenever
 * the runloop detects activity on one of the file descriptors returned by the
 * file descriptor callback. */
void
cld_runloop_callback_activity (struct cld_runloop *loop, cld_runloop_activity_func_t func, void *data)
{
	loop->activity = func;
	loop->activity_data = data;
}

/** Sets the runloop's idle callback function, which is executed every time the
 * runloop performs one iteration without having any file descriptors active. */
void
cld_runloop_callback_idle (struct cld_runloop *loop, cld_runloop_idle_func_t func, void *data)
{
	loop->idle = func;
	loop->idle_data = data;
}