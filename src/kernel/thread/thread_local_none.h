/**
 * @file
 *
 * @date Oct 7, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_LOCAL_HEAP_H_
#define THREAD_LOCAL_HEAP_H_

#include <errno.h>


struct pthread_local {
};
typedef struct pthread_local __thread_local_t;

static inline int thread_local_alloc(struct thread *t, size_t size) {
	(void)t; (void)size;
	return 0;
}

static inline void *thread_local_get(struct thread *t, size_t idx) {
	(void)t; (void)idx;
	return NULL;
}

static inline int thread_local_set(struct thread *t, size_t idx, void *value) {
	(void)t; (void)idx; (void)value;
	return -ENOSUPP;
}

static inline int thread_local_free(struct thread *t) {
	(void)t;
	return -ENOSUPP;
}

#endif /* THREAD_LOCAL_HEAP_H_ */
