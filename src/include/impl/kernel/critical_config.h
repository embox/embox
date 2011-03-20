/**
 * @file
 * @brief TODO
 *
 * @date 30.05.2010
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_H_
# error "Do not include this file directly, use <kernel/critical.h> instead!"
#endif /* KERNEL_CRITICAL_H_ */

/* Critical levels mask. */

#define __CRITICAL_HARDIRQ CRITICAL_VALUE_LOWEST(8)
#define __CRITICAL_SOFTIRQ CRITICAL_VALUE(4, __CRITICAL_HARDIRQ)
#define __CRITICAL_PREEMPT CRITICAL_VALUE(4, __CRITICAL_SOFTIRQ)

