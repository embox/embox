/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.03.2013
 */

#ifndef USERMODE86_KERNEL_UMIRQ_H_
#define USERMODE86_KERNEL_UMIRQ_H_

typedef int (*irqhnd_t)(int num);

extern void irq_entry(int num);

#endif /* USERMODE86_KERNEL_IRQ_H_ */

