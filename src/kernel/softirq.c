/**
 * @file
 * @brief Software interrupts subsystem.
 *
 * @date 24.12.09
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Rewriting from scratch:
 *          - Implementing new interface
 *          - Introducing interrupt safety
 */

#include <stddef.h>
#include <stdint.h>
#include <errno.h>

#include <kernel/softirq.h>
#include <kernel/critical.h>
#include <hal/ipl.h>
#include <util/bit.h>

static void softirq_dispatch(void);

CRITICAL_DISPATCHER_DEF(softirq_critical, softirq_dispatch,
		CRITICAL_SOFTIRQ_LOCK);

struct softirq_action {
	softirq_handler_t handler;
	void *data;
};

static struct softirq_action softirq_actions[SOFTIRQ_NRS_TOTAL];
static unsigned long softirq_pending;

int softirq_install(unsigned int nr, softirq_handler_t handler, void *data) {
	ipl_t ipl;

	if (!softirq_nr_valid(nr)) {
		return -EINVAL;
	}

	ipl = ipl_save();
	softirq_actions[nr].handler = handler;
	softirq_actions[nr].data = data;
	ipl_restore(ipl);

	return 0;
}

int softirq_raise(unsigned int nr) {
	ipl_t ipl;

	if (!softirq_nr_valid(nr)) {
		return -EINVAL;
	}

	ipl = ipl_save();
	softirq_pending |= (1 << nr);
	ipl_restore(ipl);

	critical_request_dispatch(&softirq_critical);

	return 0;
}

/**
 * Called by critical dispatching code with all hardware IRQs disabled.
 * Note that it must by reentrant by design.
 */
static void softirq_dispatch(void) {
	static unsigned long softipl = ~0x0ul; /* permit anything */

	critical_enter(CRITICAL_SOFTIRQ_HANDLER);
	{
		unsigned long saved_softipl = softipl;
		unsigned long mask;
		unsigned int nr;

		while ((mask = softirq_pending & saved_softipl)) {
			/* Handle softirqs permitted by the softipl of this context. */
			bit_foreach(nr, mask) {
				softirq_handler_t handler;
				void *data;

				softirq_pending &= ~(0x1ul << nr); /* mark as handled */

				if ((handler = softirq_actions[nr].handler)) {
					data = softirq_actions[nr].data;

					softipl = (0x1ul << nr) - 1; /* mask out lower softirqs */

					ipl_enable();
					handler(nr, data);
					ipl_disable();
				}
			}
		}

		softipl = saved_softipl;
	}
	critical_leave(CRITICAL_SOFTIRQ_HANDLER);
}
