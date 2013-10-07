/**
 * @file
 *
 * @date Oct 7, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_LOCAL_HEAP_H_
#define THREAD_LOCAL_HEAP_H_

#include <errno.h>


struct thread_local {
};
typedef struct thread_local __thread_local_t;

static inline int thread_local_alloc(struct thread *t, size_t size) {
	return -ENOSUPP;
}

static inline void *thread_local_get(struct thread *t, size_t idx) {
	return NULL;
}

static inline int thread_local_set(struct thread *t, size_t idx, void *value) {
	return -ENOSUPP;
}

#endif /* THREAD_LOCAL_HEAP_H_ */
