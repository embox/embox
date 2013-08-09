/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 */

#include <kernel/thread.h>
#include <kernel/thread/state.h>
#include <kernel/sched/sched_priority.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/sched/runq.h>
#include <kernel/task.h>
#include <kernel/sched.h>

#define prior_field(field)   t->sched_attr.thread_priority.field

int thread_priority_init(struct thread *t, sched_priority_t new_priority) {
	assert(t);
	assert(t->state == __THREAD_STATE_INIT);

	prior_field(base_priority) = new_priority;
	prior_field(current_priority) = new_priority;

	return 0;
}

int thread_priority_set(struct thread *t, sched_priority_t new_priority) {
	assert(t);

	sched_lock();
	{
		if (prior_field(base_priority) == new_priority) {
			goto out;
		}

		/* if we work with scheduling thread we must take into consideration
		 * that priority can be changed due to inheritance
		 */
		if (prior_field(base_priority) == prior_field(current_priority)) {
			prior_field(base_priority) = new_priority;
			prior_field(current_priority) = new_priority;
		} else {
			prior_field(base_priority) = new_priority;
			if (prior_field(current_priority) <= new_priority) {
				prior_field(current_priority) = new_priority;
			}
		}
	}

out:
	sched_unlock();

	return 0;
}

sched_priority_t thread_priority_get(struct thread *t) {
	assert(t);

	return prior_field(current_priority);
}

sched_priority_t thread_priority_inherit(struct thread *t,
		sched_priority_t priority) {
	assert(t);

	if(priority > prior_field(current_priority)) {
		prior_field(current_priority) = priority;
	}

	return prior_field(current_priority);
}

sched_priority_t thread_priority_reverse(struct thread *t) {
	assert(t);

	prior_field(current_priority) = prior_field(base_priority);

	return prior_field(current_priority);
}
