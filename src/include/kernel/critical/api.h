/**
 * @file
 * @brief Declares critical API.
 *
 * @date 16.05.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_API_H_
#define KERNEL_CRITICAL_API_H_

/* Critical levels mask. */

#define CRITICAL_IRQ_LOCK        0x0000003f
#define CRITICAL_IRQ_HANDLER     0x00000fc0
#define CRITICAL_SOFTIRQ_LOCK    0x0003f000
#define CRITICAL_SOFTIRQ_PENDING 0x00040000 /* Skip one bit. */
#define CRITICAL_SOFTIRQ_HANDLER 0x01f80000
#define CRITICAL_SCHED_LOCK      0x7e000000
#define CRITICAL_SCHED_PENDING   0x80000000 /* Skip one bit. */

#include __impl_x(kernel/critical/critical_impl.h)

typedef __critical_t critical_t;

extern int critical_allows(critical_t level);
extern int critical_inside(critical_t level);
extern void critical_enter(critical_t level);
extern void critical_leave(critical_t level);

extern void critical_request_dispatch(critical_t level);
extern void critical_check_dispatch(critical_t level);
extern int critical_pending_dispatch(critical_t level);

#endif /* KERNEL_CRITICAL_API_H_ */
