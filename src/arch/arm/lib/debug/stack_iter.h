/**
 * @file stack_iter.h
 * @brief 
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-29
 */

#ifndef ARM_STACK_ITER_H_
#define ARM_STACK_ITER_H_

/* Frame consist of 4 registers saved on the stack {fp, ip, lr, pc}.
 * But currently fp and lr are enough for our backtrace */
typedef struct stack_iter {
	void *fp;	/* Frame pointer */
	void *lr;	/* Link register */
} stack_iter_t;

#endif /* ARM_STACK_ITER_H_ */
