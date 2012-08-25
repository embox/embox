/**
 * @file
 * @brief Defines stack iterator type for sparc.
 *
 * @date 25 Aug, 2012
 * @author Anton Bulychev
 */

#ifndef SPARC_STACK_ITER_H_
#define SPARC_STACK_ITER_H_

#include <asm/stack.h>

typedef struct stack_iter {
	void* fp;
	void* pc;
	struct stack_frame* sf;
} stack_iter_t;

#endif /* SPARC_STACK_ITER_H_ */
