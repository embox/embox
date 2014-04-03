/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 */

#include <kernel/runnable/runnable.h>
#include <kernel/runnable/runnable_priority.h>

#include <kernel/sched/sched_priority.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/sched/runq.h>
#include <kernel/task.h>
#include <kernel/sched.h>

#define prior_field(field)   r->sched_attr.thread_priority.field

int runnable_priority_init(struct runnable *r, sched_priority_t new_priority) {
	assert(r);

	prior_field(base_priority) = new_priority;
	prior_field(current_priority) = new_priority;

	return 0;
}

int runnable_priority_set(struct runnable *r, sched_priority_t new_priority) {
	assert(r);

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

sched_priority_t runnable_priority_get(struct runnable *r) {
	assert(r);

	return prior_field(current_priority);
}

sched_priority_t runnable_priority_inherit(struct runnable *r,
		sched_priority_t priority) {
	assert(r);

	if(priority > prior_field(current_priority)) {
		prior_field(current_priority) = priority;
	}

	return prior_field(current_priority);
}

sched_priority_t runnable_priority_reverse(struct runnable *r) {
	assert(r);

	prior_field(current_priority) = prior_field(base_priority);

	return prior_field(current_priority);
}
