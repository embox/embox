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

static void softirq_dispatch(void);

CRITICAL_DISPATCHER_DEF(softirq_critical, softirq_dispatch,
		CRITICAL_SOFTIRQ_LOCK);

struct softirq_action {
	softirq_handler_t handler;
	void *data;
};

static struct softirq_action softirq_actions[SOFTIRQ_NRS_TOTAL];
static uint32_t softirq_pending;
static uint32_t softirq_handling;

typedef uint32_t softipl_t;


static softipl_t softipl_save(softirq_nr_t nm) {
	softipl_t sipl = softirq_handling;

	softirq_handling = 1 << nm;

	return sipl;
}

static void softipl_restore(softipl_t sipl) {
	softirq_handling = sipl;
}


int softirq_install(softirq_nr_t nr, softirq_handler_t handler, void *data) {
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

int softirq_raise(softirq_nr_t nr) {
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

static softirq_nr_t softirq_get_dispatch_nm(void) {
	softirq_nr_t nm;
	uint32_t pending;

	if(0 == (pending = softirq_pending)) {
		return -1;
	}

	for (nm = 0; pending & (softirq_handling - 1); pending >>= 1, ++nm) {
		if (pending & 0x1) {
			return nm;
		}
	}

	return -1;
}

/**
 * Called by critical dispatching code with max IPL (all IRQ disabled).
 */
static void softirq_dispatch(void) {
	softirq_handler_t handler;
	void *data;
	softirq_nr_t nr;
	softipl_t sipl;

	critical_enter(CRITICAL_SOFTIRQ_HANDLER);

	while (-1 != (nr = softirq_get_dispatch_nm())) {
		softirq_pending &= ~(1 << nr);
		if ((handler = softirq_actions[nr].handler)) {
			data = softirq_actions[nr].data;

			sipl = softipl_save(nr);
			ipl_enable();

			handler(nr, data);

			ipl_disable();
			softipl_restore(sipl);
		}
	}

	critical_leave(CRITICAL_SOFTIRQ_HANDLER);
}
