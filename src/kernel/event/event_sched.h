/**
 * @file
 * @brief
 *
 * @date 14.05.13
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#ifndef KERNEL_EVENT_EVENT_SCHED_H_
#define KERNEL_EVENT_EVENT_SCHED_H_

#include <kernel/sched.h>
#include <kernel/sched/waitq.h>

__BEGIN_DECLS

#define EVENT_TIMEOUT_INFINITE \
	SCHED_TIMEOUT_INFINITE

struct event {
	struct waitq waitq;
};

#define __EVENT_WAIT(event, cond_expr, timeout, intr)     \
	SCHED_WAIT_ON(&(event)->waitq, cond_expr, timeout, intr)

__END_DECLS

#endif /* KERNEL_EVENT_EVENT_SCHED_H_ */
