/**
 * @file stack_iter.h
 * @brief 
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-29
 */

#ifndef ARM_STACK_ITER_H_
#define ARM_STACK_ITER_H_

typedef struct stack_iter {
	void *fp;	/* Frame pointer */
	void *lr;	/* Link register */
	void *pc;	/* Program counter */
} stack_iter_t;

#endif /* ARM_STACK_ITER_H_ */
