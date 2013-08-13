/*
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#ifndef FIXED_STACK_H_
#define FIXED_STACK_H_

#include <stddef.h>

struct thread_stack {
	void              *stack;         /**< Allocated thread stack buffer. */
	size_t             stack_sz;      /**< Stack size. */
};

typedef struct thread_stack __thread_stack_t;

#endif /* FIXED_STACK_H_ */
