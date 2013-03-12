/**
 * @file
 * @brief TODO documentation for sched_priority.h -- Eldar Abusalimov
 *
 * @date Sep 6, 2011
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SCHED_PRIORITY_H_
#define KERNEL_THREAD_SCHED_PRIORITY_H_

#define THREAD_PRIORITY_MIN 0   /**< The lowest priority is 0. */
#define THREAD_PRIORITY_MAX 255 /**< The highest priority. */

/** Total amount of valid priorities. */
#define THREAD_PRIORITY_TOTAL \
	(THREAD_PRIORITY_MAX - THREAD_PRIORITY_MIN + 1)

#define THREAD_PRIORITY_NORMAL \
	(THREAD_PRIORITY_MIN + THREAD_PRIORITY_MAX) / 2

#define THREAD_PRIORITY_LOW  \
	(THREAD_PRIORITY_MIN + THREAD_PRIORITY_NORMAL) / 2
#define THREAD_PRIORITY_HIGH \
	(THREAD_PRIORITY_MAX + THREAD_PRIORITY_NORMAL) / 2

#define THREAD_PRIORITY_DEFAULT \
	THREAD_PRIORITY_NORMAL

typedef unsigned int sched_priority_t;

#include <kernel/task.h>
#include <kernel/thread/types.h>

#define SCHED_PRIORITY_MIN (0)
#define SCHED_PRIORITY_MAX (TASK_PRIORITY_TOTAL * THREAD_PRIORITY_TOTAL)
#define SCHED_PRIORITY_TOTAL \
	(SCHED_PRIORITY_MAX - SCHED_PRIORITY_MIN + 1)

static inline sched_priority_t get_sched_priority(task_priority_t task_priority,
		__thread_priority_t thread_priority) {
	return (sched_priority_t)(TASK_PRIORITY_MAX - task_priority) * THREAD_PRIORITY_TOTAL
		+ (sched_priority_t)(thread_priority - THREAD_PRIORITY_MIN);
}

#endif /* KERNEL_THREAD_SCHED_PRIORITY_H_ */
