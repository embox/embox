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

typedef struct runnable_priority runnable_priority_t;

#include <module/embox/kernel/sched/strategy/priority_api.h>

extern int thread_priority_init(struct thread *t, sched_priority_t priority);

extern int thread_priority_set(struct thread *t, sched_priority_t new_priority);

extern sched_priority_t thread_priority_get(struct thread *t);

extern sched_priority_t thread_priority_inherit(struct thread *t,
		sched_priority_t priority);

extern sched_priority_t thread_priority_reverse(struct thread *t);

#endif /* KERNEL_SCHED_SCHED_PRIORITY_H_ */
