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
#include <kernel/critical/api.h>

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
	if (!critical_inside(__CRITICAL_HARDIRQ)) {
		__irq_critical_ipl = ipl_save();
	}

	critical_enter(__CRITICAL_HARDIRQ);
}

/**
 * Unlock hardirq and to came out from critical section.
 * Must be called on the previously locked irq only
 *
 * @see irq_lock()
 */
static inline void irq_unlock(void) {
	extern ipl_t __irq_critical_ipl;
	critical_leave(__CRITICAL_HARDIRQ);

	if (!critical_inside(__CRITICAL_HARDIRQ)) {
		ipl_restore(__irq_critical_ipl);
	}

	critical_check_pending(__CRITICAL_HARDIRQ);
}

#endif /* KERNEL_IRQ_CRITICAL_H_ */
