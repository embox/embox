/**
 *  @brief Implements irq interface.
 *  @details This header in use if and only if
 *           mod 'irq' is not enable.
 *  @date 13.07.11
 */

#include <kernel/irq.h>
#include <kernel/critical/api.h>

void irq_lock() {
	if (!critical_inside(__CRITICAL_HARDIRQ)) {
		ipl = ipl_save();
	}

	critical_enter(__CRITICAL_HARDIRQ);
}

void irq_unlock() {
	critical_leave(__CRITICAL_HARDIRQ);

	if (!critical_inside(__CRITICAL_HARDIRQ)) {
		ipl_restore(ipl);
	}

	critical_check_pending(__CRITICAL_HARDIRQ);
}
