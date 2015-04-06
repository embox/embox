/**
 * @file
 *
 * @author Andrey Kokorev
 */

#ifndef KERNEL_LTHREAD_PRIORITY_H_
#define KERNEL_LTHREAD_PRIORITY_H_

#include <kernel/sched/sched_priority.h>
#include <kernel/lthread/lthread.h>

#define LTHREAD_PRIORITY_MIN SCHED_PRIORITY_MIN
#define LTHREAD_PRIORITY_MAX SCHED_PRIORITY_MAX

/** Total amount of valid priorities. */
#define LTHREAD_PRIORITY_TOTAL \
	(LTHREAD_PRIORITY_MAX - LTHREAD_PRIORITY_MIN + 1)

#define LTHREAD_PRIORITY_DEFAULT \
	LTHREAD_PRIORITY_MAX

#endif /* KERNEL_LTHREAD_PRIORITY_H_ */
