/**
 * @file
 * @brief TODO documentation for sched_priority.h -- Eldar Abusalimov
 *
 * @date Sep 6, 2011
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_SCHED_SCHED_PRIORITY_H_
#define KERNEL_SCHED_SCHED_PRIORITY_H_

typedef unsigned int sched_priority_t;

#define SCHED_PRIORITY_MIN (0)
#define SCHED_PRIORITY_MAX 255
#define SCHED_PRIORITY_TOTAL \
	(SCHED_PRIORITY_MAX - SCHED_PRIORITY_MIN + 1)

#include <kernel/sched/schedee_priority.h>

#if 0
#include <kernel/thread/thread_priority.h>
#include <kernel/task/task_priority.h>

static inline sched_priority_t sched_priority_full(task_priority_t tsk_p,
		sched_priority_t thread_pr) {
	int task_offset = (tsk_p - TASK_PRIORITY_MIN) * THREAD_PRIORITY_TOTAL;

	return task_offset + (thread_pr - THREAD_PRIORITY_MIN);
}

static inline sched_priority_t sched_priority_thread(task_priority_t tsk_p,
		sched_priority_t thread_pr) {
	int task_offset = (tsk_p - TASK_PRIORITY_MIN) * THREAD_PRIORITY_TOTAL;

	return thread_pr - task_offset;
}
#endif

#endif /* KERNEL_SCHED_SCHED_PRIORITY_H_ */
