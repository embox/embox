/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <util/dlist.h>

#include <kernel/runnable/runnable.h>
#include <kernel/sched/sched_strategy.h>

void runq_item_init(runq_item_t *runq_link) {
	dlist_head_init(runq_link);
}

void runq_init(runq_t *queue) {
	dlist_init(queue);
}

void runq_insert(runq_t *queue, struct runnable *runnable) {
	dlist_add_prev(&runnable->sched_attr.runq_link, queue);
}

void runq_remove(runq_t *queue, struct runnable *runnable) {
	dlist_del(&runnable->sched_attr.runq_link);
}

struct runnable *runq_extract(runq_t *queue) {
	struct runnable *runnable;

	runnable = dlist_entry(queue->next, struct runnable, sched_attr.runq_link);
	runq_remove(queue, runnable);

	return runnable;
}
