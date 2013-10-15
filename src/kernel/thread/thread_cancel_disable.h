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

static inline int thread_cancel(struct thread *t) {
	return -ENOSUPP;
}

static inline int thread_cancel_set_state(int state, int *oldstate) {
	return -ENOSUPP;
}

static inline int thread_cancel_set_type(int type, int *oldtype) {
	return -ENOSUPP;
}

static inline int thread_cancel_cleanup_push(void (*r)(void *), void *arg) {
	return -ENOSUPP;
}

static inline int thread_cancel_cleanup_pop(int execute) {
	return -ENOSUPP;
}


#endif /* THREAD_CANCEL_DISABLE_H_ */
