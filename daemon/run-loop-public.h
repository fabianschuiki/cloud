/*
 * Copyright © 2012 Fabian Schuiki
 */

struct cld_runloop;

struct cld_runloop *cld_runloop_create();
void cld_runloop_destroy(struct cld_runloop *loop);

void cld_runloop_set_timeout(struct cld_runloop *loop, int timeout);
int cld_runloop_run(struct cld_runloop *loop);

typedef struct cld_fd *(*cld_runloop_fd_func_t) (int *count, void *data);
typedef void (*cld_runloop_activity_func_t) (struct cld_fd *fds, int count, void *data);
typedef void (*cld_runloop_idle_func_t) (void *data);

void cld_runloop_callback_fd(struct cld_runloop *loop, cld_runloop_fd_func_t func, void *data);
void cld_runloop_callback_activity(struct cld_runloop *loop, cld_runloop_activity_func_t func, void *data);
void cld_runloop_callback_idle(struct cld_runloop *loop, cld_runloop_idle_func_t func, void *data);
