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
#define CRITICAL_SOFTIRQ_HANDLER  0x00fc0000
#define CRITICAL_SCHED_LOCK       0x3f000000

#include __impl_x(kernel/critical/critical_impl.h)

typedef __critical_t critical_t;

struct critical_dispatcher {
	struct critical_dispatcher *next;
	__critical_t mask;
	void (*dispatch)(void);
};

#define CRITICAL_DISPATCHER_DEF(name, dispatch_fn, critical_mask) \
	static struct critical_dispatcher name = {       \
		.dispatch = (dispatch_fn),                   \
		.mask = ~((critical_mask)                    \
				| __CRITICAL_HARDER(critical_mask)), \
	}

extern int critical_allows(critical_t level);
extern int critical_inside(critical_t level);
extern void critical_enter(critical_t level);
extern void critical_leave(critical_t level);

static inline int critical_pending(struct critical_dispatcher *d) {
	return d->mask & 0x1;
}

extern void critical_request_dispatch(struct critical_dispatcher *d);
extern void critical_dispatch_pending(void);

#endif /* KERNEL_CRITICAL_API_H_ */

