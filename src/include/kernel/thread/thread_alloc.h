/**
 * @file
 *
 * @date Jul 23, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_ALLOC_H_
#define THREAD_ALLOC_H_

#include <stddef.h>

extern struct thread *thread_alloc(size_t stack_sz);

extern void thread_free(struct thread *t);

#endif /* THREAD_ALLOC_H_ */
