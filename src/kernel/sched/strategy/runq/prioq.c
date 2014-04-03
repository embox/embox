/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <kernel/thread.h>
#include <kernel/sched/sched_strategy.h>

#include <util/priolist.h>

void runq_item_init(runq_item_t *runq_link) {
	priolist_link_init(runq_link);
}

void runq_init(runq_t *queue) {
	priolist_init(queue);
}

void runq_insert(runq_t *queue, struct thread *t) {
	t->sched_attr.runq_link.prio = -thread_priority_get(t);
	priolist_add(&t->sched_attr.runq_link, queue);
}

void runq_remove(runq_t *queue, struct thread *t) {
	priolist_del(&t->sched_attr.runq_link, queue);
}

struct thread *runq_extract(runq_t *queue) {
	runq_item_t *first = priolist_first(queue);

	priolist_del(first, queue);

	return mcast_out(first, struct thread, sched_attr.runq_link);
}
