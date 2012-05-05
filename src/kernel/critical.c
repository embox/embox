/**
 * @file
 * @brief TODO
 *
 * @date 25.03.11
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <stddef.h>

#include <kernel/critical.h>
#include <hal/ipl.h>

critical_t __critical_count = 0;

static struct critical_dispatcher *dispatch_queue;

void critical_dispatch_pending(void) {
	struct critical_dispatcher *d;
	critical_t mask;
	critical_t count = __critical_count_get();
	ipl_t ipl;

	ipl = ipl_save();

	while ((d = dispatch_queue) && !((mask = d->mask) & count)) {
		dispatch_queue = d->next;
		d->mask = ~mask;

		d->dispatch();
	}

	ipl_restore(ipl);
}

void critical_request_dispatch(struct critical_dispatcher *d) {
	struct critical_dispatcher **pp = &dispatch_queue;
	critical_t inv_mask;
	ipl_t ipl;

	assert(d != NULL);

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

