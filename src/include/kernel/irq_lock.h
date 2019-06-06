/**
 * @file
 * @brief TODO
 *
 * @date 13.07.11
 * @author Alexandr Kalmuk
 */

#ifndef KERNEL_IRQ_LOCK_H_
#define KERNEL_IRQ_LOCK_H_

#include <hal/ipl.h>
#include <kernel/critical.h>
#include <util/lang.h>

/**
 * Locks hardware interrupt.
 *
 * When hardirq locked do not call sched_dispatch.
 * This function will be called after hardirq_unlock().
 *
 * Each lock must be balanced with the corresponding unlock.
 */
static inline void irq_lock(void) {
	extern ipl_t __irq_lock_ipl;
	ipl_t tmp_ipl;
	int irq_locked;

	tmp_ipl = ipl_save();
	irq_locked = critical_inside(CRITICAL_IRQ_LOCK);
	critical_enter(CRITICAL_IRQ_LOCK);
	if (!irq_locked) {
		__irq_lock_ipl = tmp_ipl;
	} else {
		ipl_restore(tmp_ipl);
	}
}

/**
 * Unlock hardirq and to came out from critical section.
 * Must be called on the previously locked irq only.
 *
 * @see irq_lock()
 */
static inline void irq_unlock(void) {
	extern ipl_t __irq_lock_ipl;

	critical_leave(CRITICAL_IRQ_LOCK);

	if (!critical_inside(CRITICAL_IRQ_LOCK)) {
		ipl_restore(__irq_lock_ipl);
		/* We know there is no level more critical than the IRQ lock. */
		critical_dispatch_pending();
	}
}

/**
 * Evaluate a given @a expr inside an IRQ-protected block.
 */
#define IRQ_LOCKED_DO(expr) \
	__lang_surround(expr, irq_lock(), irq_unlock())

#endif /* KERNEL_IRQ_LOCK_H_ */
