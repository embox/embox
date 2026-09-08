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
#include <kernel/cpu/cpudata.h>
#include <util/lang.h>

/* The interrupt state irq_lock() saves is the saving core's own. It used to
 * be one global, so two cores in irq_lock() at once handed each other their
 * DAIF. */
extern ipl_t __irq_lock_ipl __cpudata__;

/**
 * Locks hardware interrupt.
 *
 * When hardirq locked do not call sched_dispatch.
 * This function will be called after hardirq_unlock().
 *
 * Each lock must be balanced with the corresponding unlock.
 */
static inline void irq_lock(void) {
	/* Mask first. Both questions below are per-CPU -- am I already inside an
	 * irq_lock, and where do I stash the caller's interrupt state -- and this
	 * is the one place where they are asked with interrupts still on. On the
	 * nested path the save is redundant (already masked) and its value is
	 * dropped.
	 *
	 * irq_unlock() needs no such change: it is entered with the count
	 * non-zero and interrupts masked, and does not unmask until after its last
	 * cpudata read. */
	ipl_t ipl = ipl_save();

	if (!critical_inside(CRITICAL_IRQ_LOCK)) {
		cpudata_var(__irq_lock_ipl) = ipl;
	}

	critical_enter(CRITICAL_IRQ_LOCK);
}

/**
 * Unlock hardirq and to came out from critical section.
 * Must be called on the previously locked irq only.
 *
 * @see irq_lock()
 */
static inline void irq_unlock(void) {
	critical_leave(CRITICAL_IRQ_LOCK);

	if (!critical_inside(CRITICAL_IRQ_LOCK)) {
		ipl_restore(cpudata_var(__irq_lock_ipl));
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
