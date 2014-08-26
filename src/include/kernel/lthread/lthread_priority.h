/**
 * @file
 *
 * @author Andrey Kokorev
 */

#ifndef KERNEL_LTHREAD_PRIORITY_H_
#define KERNEL_LTHREAD_PRIORITY_H_

#include <kernel/sched/sched_priority.h>

#define LTHREAD_PRIORITY_MIN SCHED_PRIORITY_MIN
#define LTHREAD_PRIORITY_MAX SCHED_PRIORITY_MAX

/** Total amount of valid priorities. */
#define LTHREAD_PRIORITY_TOTAL \
	(LTHREAD_PRIORITY_MAX - LTHREAD_PRIORITY_MIN + 1)

#define LTHREAD_PRIORITY_DEFAULT \
	LTHREAD_PRIORITY_MAX

static inline int lthread_priority_set(struct lthread *t, sched_priority_t new_priority) {
	schedee_priority_set(&t->schedee.priority, new_priority);
	return 0;
}

#endif /* KERNEL_LTHREAD_PRIORITY_H_ */
