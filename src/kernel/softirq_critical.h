/**
 *  @brief Implements softirq locks interface.
 *  @details This header in use also if mod 'softirq' is not enable.
 *  @date 13.07.11
 *  @author Alexandr Kalmuk
 */

#ifndef KERNEL_SOFTIRQ_CRITICAL_H_
#define KERNEL_SOFTIRQ_CRITICAL_H_

#include <kernel/softirq.h>
#include <kernel/critical.h>

/**
 * Locks hardirq and to come in critical section.
 *
 * When hardirq locked do not call sched_dispatch().
 * sched_dispatch() will be called after hardirq_unlock().
 *
 * Each lock must be balanced with the corresponding unlock.
 *
 * @see softirq_lock()
 */
static inline void softirq_lock(void) {
	critical_enter(CRITICAL_SOFTIRQ_LOCK);
}

/**
 * Unlocks hardirq and to came out from critical section
 * Must be called on the previously locked softirq only.
 *
 * @see softirq_lock()
 */
static inline void softirq_unlock(void) {
	critical_leave(CRITICAL_SOFTIRQ_LOCK);
	critical_dispatch_pending();
}

#endif /* KERNEL_SOFTIRQ_CRITICAL_H_ */
