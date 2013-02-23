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

#include <kernel/percpu.h>
#include <kernel/critical.h>

critical_t __critical_count __percpu__ = 0;

static struct critical_dispatcher *dispatch_queue __percpu__;

void critical_dispatch_pending(void) {
	struct critical_dispatcher **pp = percpu_ptr(&dispatch_queue);
	critical_t count = percpu_var(__critical_count);
	struct critical_dispatcher *d;
	critical_t mask;
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
	struct critical_dispatcher **pp = percpu_ptr(&dispatch_queue);
	critical_t inv_mask;
	ipl_t ipl;

	assert(d != NULL);

	d = percpu_ptr(d); /* Getting dispatcher of current CPU */

	ipl = ipl_save();
	if (critical_pending(d)) {
		ipl_restore(ipl);
		return;
	}

	inv_mask = d->mask;

	while (*pp && !((*pp)->mask & inv_mask)) {
		pp = &(*pp)->next;
	}

	d->next = *pp;
	*pp = d;

	d->mask = ~inv_mask;

	ipl_restore(ipl);

	critical_dispatch_pending();
}
