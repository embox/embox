/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <lib/libds/dlist.h>

#include <kernel/sched.h>
#include <kernel/sched/sched_strategy.h>

struct schedee;

void runq_item_init(runq_item_t *runq_link) {
	dlist_head_init(runq_link);
}

void runq_init(runq_t *queue) {
	dlist_init(queue);
}

void runq_insert(runq_t *queue, struct schedee *schedee) {
	dlist_add_prev(&schedee->runq_link, queue);
}

void runq_remove(runq_t *queue, struct schedee *schedee) {
	dlist_del(&schedee->runq_link);
}

struct schedee *runq_get_next(runq_t *queue) {
	struct schedee *schedee;
	struct schedee *s;

	schedee = mcast_out(queue->next, struct schedee, runq_link);

	dlist_foreach_entry_safe(s, queue, runq_link) {
		if (schedee_priority_get(s) >  schedee_priority_get(schedee)) {
			schedee = s;
		}
	}

	return schedee;
}

struct schedee *runq_extract(runq_t *queue) {
	struct schedee *schedee;

	schedee = runq_get_next(queue);
	runq_remove(queue, schedee);

	return schedee;
}
