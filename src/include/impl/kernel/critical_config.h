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

#define __CRITICAL_HARDIRQ 0x000000ffUL
#define __CRITICAL_SOFTIRQ 0x00000f00UL
#define __CRITICAL_PREEMPT 0x0000f000UL

