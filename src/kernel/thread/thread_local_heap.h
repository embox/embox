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

struct thread_local {
	void **storage;
	size_t size;
};
typedef struct thread_local __thread_local_t;

#endif /* THREAD_LOCAL_HEAP_H_ */
