/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <util/dlist.h>

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

struct schedee *runq_extract(runq_t *queue) {
	struct schedee *schedee;

	schedee = dlist_entry(queue->next, struct schedee, runq_link);
	runq_remove(queue, schedee);

	return schedee;
}
