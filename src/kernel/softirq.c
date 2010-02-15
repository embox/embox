/**
 * @file
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <types.h>
#include <assert.h>
#include <errno.h>

#include <kernel/softirq.h>
#include <hal/ipl.h>

struct softirq_action {
	softirq_handler_t handler;
	void *dev_id;
};

static struct softirq_action softirq_actions[SOFTIRQ_NRS_TOTAL];
static uint32_t softirq_pending;

int softirq_install(softirq_nr_t nr, softirq_handler_t handler, void *dev_id) {
	ipl_t ipl;

	if (!softirq_nr_valid(nr)) {
		return -EINVAL;
	}

	ipl = ipl_save();
	softirq_actions[nr].handler = handler;
	softirq_actions[nr].dev_id = dev_id;
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

	return 0;
}

void softirq_dispatch(void) {
	ipl_t ipl;

	ipl = ipl_save();

	ipl_restore(ipl);
}
