/**
 * @file
 *
 * @date Oct 7, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_LOCAL_HEAP_H_
#define THREAD_LOCAL_HEAP_H_

static inline void *thread_local_alloc(struct thread *t, size_t size) {
	return NULL;
}

#endif /* THREAD_LOCAL_HEAP_H_ */
