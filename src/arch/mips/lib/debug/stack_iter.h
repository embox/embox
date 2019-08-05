/**
 * @brief
 * @author Nastya Nizharadze
 * @date 30.07.2019
 */

#ifndef MIPS_STACK_ITER_H_
#define MIPS_STACK_ITER_H_

typedef struct stack_iter {
	void *fp;
	void *ra;
} stack_iter_t;

#endif /* MIPS_STACK_ITER_H_ */
