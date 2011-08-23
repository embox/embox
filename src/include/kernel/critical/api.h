/**
 * @file
 * @brief Declares critical API.
 *
 * @date 16.05.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_API_H_
#define KERNEL_CRITICAL_API_H_

#include __impl_x(kernel/critical/api_impl.h)

#define CRITICAL_HARDIRQ __CRITICAL_HARDIRQ
#define CRITICAL_SOFTIRQ __CRITICAL_SOFTIRQ
#define CRITICAL_PREEMPT __CRITICAL_PREEMPT

typedef __critical_t critical_t;

extern int critical_allows(critical_t level);

extern int critical_inside(critical_t level);

extern void critical_enter(critical_t level);

extern void critical_leave(critical_t level);

extern void critical_request_dispatch(critical_t level);
extern void critical_check_dispatch(critical_t level);
extern int critical_pending_dispatch(critical_t level);

#endif /* KERNEL_CRITICAL_API_H_ */
