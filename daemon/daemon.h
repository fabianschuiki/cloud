/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_H
#define CLOUD_H


struct cld_event_loop;
struct cld_event_source;

struct cld_event_loop *cld_event_loop_create();
void cld_event_loop_destroy(struct cld_event_loop *loop);

int cld_event_loop_dispatch(struct cld_event_loop *loop);

void cld_event_source_remove(struct cld_event_source *source);

enum {
	CLD_EVENT_READABLE = (1 << 0),
	CLD_EVENT_WRITABLE = (1 << 1)
};

//Signal Events
typedef int (*cld_event_loop_signal_func_t) (int signal_number, void *data);
struct cld_event_source *cld_event_loop_add_signal(struct cld_event_loop *loop,
	int signal_number,
	cld_event_loop_signal_func_t func,
	void *data);

//FD Events
typedef int (*cld_event_loop_fd_func_t) (int fd, int mask, void *data);
struct cld_event_source *cld_event_loop_add_fd(struct cld_event_loop *loop,
	int fd,
	int mask,
	cld_event_loop_fd_func_t func,
	void *data);
int cld_event_source_fd_update(struct cld_event_source *source, int mask);

struct cld_daemon;
struct cld_client;

struct cld_client *cld_client_create(struct cld_daemon *daemon, int fd);
void cld_client_destroy(struct cld_client *client);


#endif