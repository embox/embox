/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_CANCEL_DISABLE_H_
#define THREAD_CANCEL_DISABLE_H_

struct thread_cancel {
};
typedef struct thread_cancel thread_cancel_t;

#include <errno.h>

struct thread;
static inline int thread_cancel(struct thread *t) {
	(void)t;
	return -ENOSUPP;
}

static inline int thread_cancel_set_state(int state, int *oldstate) {
	(void)state; (void)oldstate;
	return -ENOSUPP;
}

static inline int thread_cancel_set_type(int type, int *oldtype) {
	(void)type; (void)oldtype;
	return -ENOSUPP;
}

static inline int thread_cancel_cleanup_push(void (*r)(void *), void *arg) {
	(void)r; (void)arg;
	return -ENOSUPP;
}

static inline int thread_cancel_cleanup_pop(int execute) {
	(void)execute;
	return -ENOSUPP;
}

static inline int thread_cancel_init(struct thread *t) {
	(void)t;
	return -ENOSUPP;
}

#endif /* THREAD_CANCEL_DISABLE_H_ */
