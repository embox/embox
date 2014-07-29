/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 */

#include <kernel/schedee/schedee.h>
#include <kernel/schedee/schedee_priority.h>

#include <kernel/sched/sched_priority.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/sched/runq.h>
#include <kernel/task.h>
#include <kernel/sched.h>

#define prior_field(field)   s->sched_attr.thread_priority.field

int schedee_priority_init(struct schedee *s, sched_priority_t new_priority) {
	assert(s);

	prior_field(base_priority) = new_priority;
	prior_field(current_priority) = new_priority;

	return 0;
}

int schedee_priority_set(struct schedee *s, sched_priority_t new_priority) {
	assert(s);

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

sched_priority_t schedee_priority_get(struct schedee *s) {
	assert(s);

	return prior_field(current_priority);
}

sched_priority_t schedee_priority_inherit(struct schedee *s,
		sched_priority_t priority) {
	assert(s);

	if(priority > prior_field(current_priority)) {
		prior_field(current_priority) = priority;
	}

	return prior_field(current_priority);
}

sched_priority_t schedee_priority_reverse(struct schedee *s) {
	assert(s);

	prior_field(current_priority) = prior_field(base_priority);

	return prior_field(current_priority);
}
