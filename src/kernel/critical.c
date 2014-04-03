/**
 * @file
 * @brief TODO
 *
 * @date 25.03.11
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <stddef.h>

#include <hal/ipl.h>

#include <kernel/cpu/cpudata.h>
#include <kernel/critical.h>

unsigned int __critical_count __cpudata__ = 0;

static struct critical_dispatcher *dispatch_queue __cpudata__;

void critical_dispatch_pending(void) {
	struct critical_dispatcher **pp = cpudata_ptr(&dispatch_queue);
	unsigned int count = critical_count();
	struct critical_dispatcher *d;
	unsigned int mask;
	ipl_t ipl;

	ipl = ipl_save();

	while ((d = *pp) && !((mask = d->mask) & count)) {
		*pp = d->next;
		d->mask = ~mask;

		assert(d->dispatch != NULL);
		d->dispatch();
	}

	ipl_restore(ipl);
}

void critical_request_dispatch(struct critical_dispatcher *d) {
	struct critical_dispatcher **pp;
	unsigned int inv_mask;
	ipl_t ipl;

	assert(d != NULL);

	d = cpudata_ptr(d); /* Getting dispatcher of current CPU */

	ipl = ipl_save();
	if (critical_pending(d)) {
		ipl_restore(ipl);
		return;
	}

	inv_mask = d->mask;

	for (pp = cpudata_ptr(&dispatch_queue); *pp; pp = &(*pp)->next) {
		if ((*pp)->mask & inv_mask)
			break;
	}

	d->next = *pp;
	*pp = d;

	d->mask = ~inv_mask;

	ipl_restore(ipl);

	critical_dispatch_pending();
}
