/**
 * @file
 * @brief Declares critical API.
 *
 * @date 16.05.2010
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_API_H_
#define KERNEL_CRITICAL_API_H_

#include __impl_x(kernel/critical/api_impl.h)

#define CRITICAL_HARDIRQ __CRITICAL_HARDIRQ
#define CRITICAL_SOFTIRQ __CRITICAL_SOFTIRQ
#define CRITICAL_PREEMPT __CRITICAL_PREEMPT

typedef __critical_t critical_t;

#define critical_dispatch(critical) \
	  __critical_dispatch(critical)

extern int critical_allows(critical_t critical);

extern int critical_inside(critical_t critical);

extern void critical_enter(critical_t critical);

extern void critical_leave(critical_t critical);

#endif /* KERNEL_CRITICAL_API_H_ */
