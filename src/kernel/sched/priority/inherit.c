/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 * @author: Anton Kozlov
 */

#include <kernel/sched/schedee_priority.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/sched/schedee.h>
#include <kernel/sched.h>
#include <errno.h>
#include "inherit.h"

int schedee_priority_init(struct schedee *s, sched_priority_t new_priority) {
	s->priority.base_priority = new_priority;
	s->priority.current_priority = new_priority;

	return 0;
}

static int __schedee_priority_set(struct schedee_priority *p, sched_priority_t new_priority) {

	sched_lock();
	{
		if ((sched_priority_t)p->base_priority == new_priority) {
			goto out;
		}

		/* if we work with scheduling thread we must take into consideration
		 * that priority can be changed due to inheritance
		 */
		if (p->base_priority == p->current_priority) {
			p->base_priority = new_priority;
			p->current_priority = new_priority;
		} else {
			p->base_priority = new_priority;
			if ((sched_priority_t)p->current_priority <= new_priority) {
				p->current_priority = new_priority;
			}
		}
	}

out:
	sched_unlock();

	return 0;
}

int schedee_priority_set(struct schedee *s, sched_priority_t new_priority) {

	if ((new_priority < SCHED_PRIORITY_MIN)
			|| (new_priority > SCHED_PRIORITY_MAX)) {
		return -EINVAL;
	}

	return sched_change_priority(s, new_priority, &__schedee_priority_set);
}

sched_priority_t schedee_priority_get(struct schedee *s) {
	return s->priority.current_priority;
}

sched_priority_t schedee_priority_inherit(struct schedee *s,
		sched_priority_t priority) {

	if (priority > s->priority.current_priority) {
		s->priority.current_priority = priority;
	}

	return s->priority.current_priority;
}

sched_priority_t schedee_priority_reverse(struct schedee *s) {

	s->priority.current_priority = s->priority.base_priority;

	return s->priority.current_priority;
}
