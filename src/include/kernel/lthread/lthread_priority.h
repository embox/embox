/**
 * @file
 *
 * @author Andrey Kokorev
 */

#ifndef KERNEL_LTHREAD_PRIORITY_H_
#define KERNEL_LTHREAD_PRIORITY_H_

#include <kernel/sched/sched_priority.h>

#define LTHREAD_PRIORITY_MIN SCHED_PRIORITY_MAX - 255
#define LTHREAD_PRIORITY_MAX SCHED_PRIORITY_MAX

/** Total amount of valid priorities. */
#define LTHREAD_PRIORITY_TOTAL \
	(LTHREAD_PRIORITY_MAX - LTHREAD_PRIORITY_MIN + 1)

#define LTHREAD_PRIORITY_NORMAL \
	(LTHREAD_PRIORITY_MIN + LTHREAD_PRIORITY_MAX) / 2

#define LTHREAD_PRIORITY_LOW  \
	(LTHREAD_PRIORITY_MIN + LTHREAD_PRIORITY_NORMAL) / 2
#define LTHREAD_PRIORITY_HIGH \
	(LTHREAD_PRIORITY_MAX + LTHREAD_PRIORITY_NORMAL) / 2

#define LTHREAD_PRIORITY_DEFAULT \
	LTHREAD_PRIORITY_NORMAL

extern int lthread_priority_init(struct lthread *lt, sched_priority_t priority);

extern int lthread_priority_set(struct lthread *lt, sched_priority_t new_priority);

extern sched_priority_t lthread_priority_get(struct lthread *lt);

extern sched_priority_t lthread_priority_inherit(struct lthread *lt,
		sched_priority_t priority);

extern sched_priority_t lthread_priority_reverse(struct lthread *lt);

#endif /* KERNEL_LTHREAD_PRIORITY_H_ */
