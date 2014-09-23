/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 * @author: Anton Kozlov
 */

#ifndef SCHED_PRIORITY_INHERIT_H_
#define SCHED_PRIORITY_INHERIT_H_

#include <kernel/sched/schedee_priority.h>
#include <kernel/sched/sched_lock.h>

struct schedee_priority {
	short base_priority;      /**< Priority was set for the schedee. */
	short current_priority;   /**< Current schedee scheduling priority. */
};

#define prior_field(field)   p->field

static inline int schedee_priority_init(struct schedee_priority *p, sched_priority_t new_priority) {

	prior_field(base_priority) = new_priority;
	prior_field(current_priority) = new_priority;

	return 0;
}

static inline int schedee_priority_set(struct schedee_priority *p, sched_priority_t new_priority) {

	sched_lock();
	{
		if ((sched_priority_t)prior_field(base_priority) == new_priority) {
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
			if ((sched_priority_t)prior_field(current_priority) <= new_priority) {
				prior_field(current_priority) = new_priority;
			}
		}
	}

out:
	sched_unlock();

	return 0;
}

static inline sched_priority_t schedee_priority_get(struct schedee_priority *p) {

	return prior_field(current_priority);
}

static inline sched_priority_t schedee_priority_inherit(struct schedee_priority *p,
		sched_priority_t priority) {

	if(priority > (sched_priority_t)prior_field(current_priority)) {
		prior_field(current_priority) = priority;
	}

	return prior_field(current_priority);
}

static inline sched_priority_t schedee_priority_reverse(struct schedee_priority *p) {

	prior_field(current_priority) = prior_field(base_priority);

	return prior_field(current_priority);
}

#undef prior_field

#endif /* SCHED_PRIORITY_INHERIT_H_ */
