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
	ipl_t ipl;

	/* The queue head and the count belong to the CPU this thread is on now,
	 * and d->dispatch() is sched_preempt() -- which puts it on another CPU and
	 * comes back with interrupts enabled if the thread it switched to enabled
	 * them. So the pair is taken again on every turn of the loop, under a
	 * fresh mask, rather than once before it. */
	ipl = ipl_save();

	while (1) {
		struct critical_dispatcher **pp = cpudata_ptr(&dispatch_queue);
		struct critical_dispatcher *d = *pp;
		unsigned int mask;

		if (!d || ((mask = d->mask) & critical_count())) {
			break;
		}

		*pp = d->next;
		d->mask = ~mask;

		assert(d->dispatch != NULL);
		d->dispatch();

		/* May have returned on another CPU, and with interrupts on. */
		ipl_save();
	}

	ipl_restore(ipl);
}

int critical_dispatch_required(void) {
	struct critical_dispatcher **pp;
	unsigned int count;
	struct critical_dispatcher *d;
	int required;
	ipl_t ipl;

	/* Two per-CPU reads that have to name the same CPU. */
	ipl = ipl_save();
	pp = cpudata_ptr(&dispatch_queue);
	count = critical_count();
	required = ((d = *pp) && !(d->mask & count));
	ipl_restore(ipl);

	return required;
}

void critical_request_dispatch(struct critical_dispatcher *d) {
	struct critical_dispatcher **pp;
	unsigned int inv_mask;
	ipl_t ipl;

	assert(d != NULL);

	/* The dispatcher is picked by CPU, so the pick has to be inside the mask
	 * -- otherwise a migration here queues this CPU's work onto the one we
	 * left. */
	ipl = ipl_save();
	d = cpudata_ptr(d); /* Getting dispatcher of current CPU */

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
