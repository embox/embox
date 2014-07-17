/**
 * @file
 *
 * @author Andrey Kokorev
 */

#ifndef KERNEL_LWTHREAD_PRIORITY_H_
#define KERNEL_LWTHREAD_PRIORITY_H_

#include <kernel/sched/sched_priority.h>

#define LWTHREAD_PRIORITY_MIN SCHED_PRIORITY_MAX - 255   /**< The lowest priority is 0. */
#define LWTHREAD_PRIORITY_MAX SCHED_PRIORITY_MAX /**< The highest priority. */

/** Total amount of valid priorities. */
#define LWTHREAD_PRIORITY_TOTAL \
	(LWTHREAD_PRIORITY_MAX - LWTHREAD_PRIORITY_MIN + 1)

#define LWTHREAD_PRIORITY_NORMAL \
	(LWTHREAD_PRIORITY_MIN + LWTHREAD_PRIORITY_MAX) / 2

#define LWTHREAD_PRIORITY_LOW  \
	(LWTHREAD_PRIORITY_MIN + LWTHREAD_PRIORITY_NORMAL) / 2
#define LWTHREAD_PRIORITY_HIGH \
	(LWTHREAD_PRIORITY_MAX + LWTHREAD_PRIORITY_NORMAL) / 2

#define LWTHREAD_PRIORITY_DEFAULT \
	LWTHREAD_PRIORITY_NORMAL

extern int lwthread_priority_init(struct lthread *lwt, sched_priority_t priority);

extern int lwthread_priority_set(struct lthread *lwt, sched_priority_t new_priority);

extern sched_priority_t lwthread_priority_get(struct lthread *lwt);

extern sched_priority_t lwthread_priority_inherit(struct lthread *lwt,
		sched_priority_t priority);

extern sched_priority_t lwthread_priority_reverse(struct lthread *lwt);

#endif /* KERNEL_LWTHREAD_PRIORITY_H_ */
