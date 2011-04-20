/**
 * @file
 * @brief TODO
 *
 * @date Apr 18, 2011
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_DISPATCH_H_
#define KERNEL_CRITICAL_DISPATCH_H_

#if 0
#define __critical_dispatch(critical) \
	((critical) == __CRITICAL_HARDIRQ ? __critical_dispatch_hardirq() : \
	 (critical) == __CRITICAL_SOFTIRQ ? __critical_dispatch_softirq() : \
	 (critical) == __CRITICAL_PREEMPT ? __critical_dispatch_preempt() : \
			__critical_dispatch_error())

static inline void __critical_dispatch_hardirq(void) {
}
#endif

#endif /* KERNEL_CRITICAL_DISPATCH_H_ */
