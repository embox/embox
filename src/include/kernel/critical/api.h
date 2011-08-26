/**
 * @file
 * @brief Declares critical API.
 *
 * @date 16.05.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_API_H_
#define KERNEL_CRITICAL_API_H_

#include <assert.h>
#include <stddef.h>

/* Critical levels mask. */

#define CRITICAL_IRQ_LOCK         0x0000003f
#define CRITICAL_IRQ_HANDLER      0x00000fc0
#define CRITICAL_SOFTIRQ_LOCK     0x0003f000
#define CRITICAL_SOFTIRQ_DISPATCH 0x00040000
#define CRITICAL_SOFTIRQ_HANDLER  0x01f80000
#define CRITICAL_SCHED_LOCK       0x7e000000
#define CRITICAL_SCHED_DISPATCH   0x80000000

#include __impl_x(kernel/critical/critical_impl.h)

typedef __critical_t critical_t;

struct critical_dispatcher {
	struct critical_dispatcher *next;
	__critical_t mask;
	void (*dispatch)(void);
};

#define CRITICAL_DISPATCHER_DEF(name, dispatch_fn, critical_mask) \
	static struct critical_dispatcher name = { \
		.dispatch = (dispatch_fn), \
		.mask = ~(critical_mask),   \
	}

extern int critical_allows(critical_t level);
extern int critical_inside(critical_t level);
extern void critical_enter(critical_t level);
extern void critical_leave(critical_t level);

static inline void critical_dispatch_pending(void) {
	extern struct critical_dispatcher *__critical_dispatch_queue;
	struct critical_dispatcher *d;
	__critical_t mask;
	__critical_t count = __critical_count_get();
	ipl_t ipl;

	ipl = ipl_save();

	while ((d = __critical_dispatch_queue) && !((mask = d->mask) & count)) {
		__critical_dispatch_queue = d->next;
		d->mask = ~mask;

		d->dispatch();
	}

	ipl_restore(ipl);
}

static inline int critical_pending(struct critical_dispatcher *d) {
	return d->mask & 0x1;
}

static inline void critical_request_dispatch(struct critical_dispatcher *d) {
	extern struct critical_dispatcher *__critical_dispatch_queue;
	struct critical_dispatcher **pp = &__critical_dispatch_queue;
	__critical_t inv_mask;
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

extern void critical_request_dispatch(struct critical_dispatcher *d);
extern void critical_dispatch_pending(void);

#endif /* KERNEL_CRITICAL_API_H_ */

