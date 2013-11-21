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

void runq_queue_init(runq_queue_t *queue) {
	dlist_init(queue);
}

void runq_queue_insert(runq_queue_t *queue, struct runnable *runnable) {
	dlist_add_prev(&runnable->sched_attr.runq_link, queue);
}

void runq_queue_remove(runq_queue_t *queue, struct runnable *runnable) {
	dlist_del(&runnable->sched_attr.runq_link);
}

struct runnable *runq_queue_extract(runq_queue_t *queue) {
	struct runnable *runnable;

	runnable = dlist_entry(queue->next, struct runnable, sched_attr.runq_link);
	runq_queue_remove(queue, runnable);

	return runnable;
}

