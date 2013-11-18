/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <util/dlist.h>

#include <kernel/thread.h>
#include <kernel/sched/sched_strategy.h>

void runq_item_init(runq_item_t *runq_link) {
	dlist_head_init(runq_link);
}

void runq_init(runq_t *queue) {
	dlist_init(queue);
}

void runq_insert(runq_t *queue, struct thread *thread) {
	dlist_add_prev(&thread->sched_attr.runq_link, queue);
}

void runq_remove(runq_t *queue, struct thread *thread) {
	dlist_del(&thread->sched_attr.runq_link);
}

struct thread *runq_extract(runq_t *queue) {
	struct thread *thread;

	thread = dlist_entry(queue->next, struct thread, sched_attr.runq_link);
	runq_remove(queue, thread);

	return thread;
}

