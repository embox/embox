/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.07.2013
 */

#ifndef KERNEL_IRQ_STACK_H_
#define KERNEL_IRQ_STACK_H_

/**
 * @brief Checks for thread's stack overflow.
 *
 * @return not-zero if overflow detetected
 * @return zero if no overflow detected.
 */
extern int irq_stack_protection(void);

#endif /* KERNEL_IRQ_STACK_H_ */



