/**
 *  @brief Implements irq locks interface.
 *  @details This header in use also if mod 'irq' is not enable.
 *  @date 13.07.11
 *  @author Alexandr Kalmuk
 */

#ifndef KERNEL_IRQ_CRITICAL_H_
#define KERNEL_IRQ_CRITICAL_H_

#include <kernel/irq.h>
#include <hal/ipl.h>
#include <kernel/critical.h>

/**
 * Locks hardirq and to come in critical section.
 *
 * When hardirq locked do not call softirq_dispatch, sched_dispatch.
 * This function will be called after hardirq_unlock().
 *
 * Each lock must be balanced with the corresponding unlock.
 * @param critical
 */
static inline void irq_lock(void) {
	extern ipl_t __irq_critical_ipl;
	if (!critical_inside(CRITICAL_IRQ_LOCK)) {
		__irq_critical_ipl = ipl_save();
	}

	critical_enter(CRITICAL_IRQ_LOCK);
}

/**
 * Unlock hardirq and to came out from critical section.
 * Must be called on the previously locked irq only
 *
 * @see irq_lock()
 */
static inline void irq_unlock(void) {
	extern ipl_t __irq_critical_ipl;
	critical_leave(CRITICAL_IRQ_LOCK);

	if (!critical_inside(CRITICAL_IRQ_LOCK)) {
		ipl_restore(__irq_critical_ipl);
	}

	critical_dispatch_pending();
}

#endif /* KERNEL_IRQ_CRITICAL_H_ */
