/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <kernel/runnable/runnable.h>
#include <kernel/runnable/runnable_priority.h>
#include <kernel/sched/sched_strategy.h>

#include <util/priolist.h>

void runq_item_init(runq_item_t *runq_link) {
	priolist_link_init(runq_link);
}

void runq_init(runq_t *queue) {
	priolist_init(queue);
}

void runq_insert(runq_t *queue, struct runnable *r) {
	r->sched_attr.runq_link.prio = -runnable_priority_get(r);
	priolist_add(&r->sched_attr.runq_link, queue);
}

void runq_remove(runq_t *queue, struct runnable *r) {
	priolist_del(&r->sched_attr.runq_link, queue);
}

struct runnable *runq_extract(runq_t *queue) {
	runq_item_t *first = priolist_first(queue);
	struct runnable *result;

	priolist_del(first, queue);
	result = mcast_out(first, struct runnable, sched_attr.runq_link);

	return result;
}
