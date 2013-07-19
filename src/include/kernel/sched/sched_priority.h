/**
 * @file
 * @brief TODO documentation for sched_priority.h -- Eldar Abusalimov
 *
 * @date Sep 6, 2011
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_SCHED_SCHED_PRIORITY_H_
#define KERNEL_SCHED_SCHED_PRIORITY_H_

#include <kernel/thread/thread_priority.h>
#include <kernel/task/task_priority.h>

typedef unsigned int sched_priority_t;

#define SCHED_PRIORITY_MIN (0)
#define SCHED_PRIORITY_MAX (TASK_PRIORITY_TOTAL * THREAD_PRIORITY_TOTAL)
#define SCHED_PRIORITY_TOTAL \
	(SCHED_PRIORITY_MAX - SCHED_PRIORITY_MIN + 1)

static inline sched_priority_t get_sched_priority(task_priority_t task_priority,
		__thread_priority_t thread_priority) {
	return (sched_priority_t)(TASK_PRIORITY_MAX - task_priority) * THREAD_PRIORITY_TOTAL
		+ (sched_priority_t)(thread_priority - THREAD_PRIORITY_MIN);
}

#endif /* KERNEL_SCHED_SCHED_PRIORITY_H_ */
