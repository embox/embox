/**
 * @file
 *
 * @date Oct 7, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_LOCAL_HEAP_H_
#define THREAD_LOCAL_HEAP_H_

#include <stdint.h>
#include <stddef.h>

struct pthread_local {
	void **storage;
	size_t size;
};
typedef struct pthread_local __thread_local_t;

#endif /* THREAD_LOCAL_HEAP_H_ */
