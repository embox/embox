/**
 * @file
 * @brief TODO
 *
 * @date 13.07.11
 * @author Alexandr Kalmuk
 */

#ifndef KERNEL_SOFTIRQ_LOCK_H_
#define KERNEL_SOFTIRQ_LOCK_H_

#include <kernel/critical.h>
#include <util/lang.h>

/**
 * XXX
 * Each lock must be balanced with the corresponding unlock.
 *
 * @see softirq_lock()
 */
static inline void softirq_lock(void) {
	critical_enter(CRITICAL_SOFTIRQ_LOCK);
}

/**
 * XXX
 * Must be called on the previously locked softirq only.
 *
 * @see softirq_lock()
 */
static inline void softirq_unlock(void) {
	critical_leave(CRITICAL_SOFTIRQ_LOCK);
	critical_dispatch_pending();
}

#define SOFTIRQ_LOCKED_DO(_expr) \
	__lang_surround(_expr, softirq_lock(), softirq_unlock())

#endif /* KERNEL_SOFTIRQ_LOCK_H_ */
