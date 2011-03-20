/**
 * @file
 * @brief Declares critical API
 *
 * @date 16.05.2010
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_H_
#define KERNEL_CRITICAL_H_

#include __impl(kernel/critical.h)

#define CRITICAL_HARDIRQ __CRITICAL_HARDIRQ
#define CRITICAL_SOFTIRQ __CRITICAL_SOFTIRQ
#define CRITICAL_PREEMPT __CRITICAL_PREEMPT

#define CRITICAL_VALUE(bits, lower_mask) \
	  __CRITICAL_VALUE(bits, lower_mask)

#define CRITICAL_VALUE_LOWEST(bits) \
	  __CRITICAL_VALUE_LOWEST(bits)

typedef __critical_t critical_t;

extern int critical_allows(critical_t critical);

extern int critical_inside(critical_t critical);

extern void critical_enter(critical_t critical);

extern void critical_leave(critical_t critical);

#endif /* KERNEL_CRITICAL_H_ */
