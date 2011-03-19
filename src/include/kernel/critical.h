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

extern unsigned int critical_inside_hardirq(void);

extern void critical_enter_hardirq(void);

extern void critical_leave_hardirq(void);

extern unsigned int critical_allows_softirq(void);

extern unsigned int critical_inside_softirq(void);

extern void critical_enter_softirq(void);

extern void critical_leave_softirq(void);

extern unsigned int critical_allows_preempt(void);

extern unsigned int critical_inside_preempt(void);

extern void critical_enter_preempt(void);

extern void critical_leave_preempt(void);

#endif /* KERNEL_CRITICAL_H_ */
