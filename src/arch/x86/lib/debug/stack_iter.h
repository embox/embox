/**
 * @file
 * @brief Defines stack iterator type for x86.
 *
 * @date 25 Aug, 2012
 * @author Anton Bulychev
 */

#ifndef X86_STACK_ITER_H_
#define X86_STACK_ITER_H_

typedef struct stack_iter {
	void* fp;
	void* pc;
} stack_iter_t;

#endif /* X86_STACK_ITER_H_ */
