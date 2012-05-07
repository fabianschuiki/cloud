/*
 * Copyright © 2012 Fabian Schuiki
 */

#ifndef CLOUD_H
#define CLOUD_H

#ifdef __cplusplus
extern "C" {
#endif


struct cld_event_loop;
struct cld_event_source;

struct cld_event_loop *cld_event_loop_create();
void cld_event_loop_destroy(struct cld_event_loop *loop);

int cld_event_loop_dispatch(struct cld_event_loop *loop);

//Signals
typedef int (*cld_event_loop_signal_func_t) (int signal_number, void *data);
struct cld_event_source *cld_event_loop_add_signal(struct cld_event_loop *loop,
	int signal_number,
	cld_event_loop_signal_func_t func,
	void *data);


#ifdef __cplusplus
}
#endif

#endif