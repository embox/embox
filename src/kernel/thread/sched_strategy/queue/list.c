/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <util/dlist.h>

#include <kernel/thread.h>
#include <kernel/thread/sched_strategy.h>

void sched_strategy_init(struct sched_strategy_data *s) {
	dlist_head_init(&s->l_link);
}

void runq_queue_init(runq_queue_t *queue) {
	dlist_init(queue);
}

void runq_queue_insert(runq_queue_t *queue, struct thread *thread) {
	dlist_add_prev(&thread->sched.l_link, queue);
}

void runq_queue_remove(runq_queue_t *queue, struct thread *thread) {
	dlist_del(&thread->sched.l_link);
}

struct thread *runq_queue_extract(runq_queue_t *queue) {
	struct thread *thread = dlist_entry(queue->next, struct thread, sched.l_link);
	runq_queue_remove(queue, thread);
	return thread;
}

void sleepq_queue_init(sleepq_queue_t *queue) {
	dlist_init(queue);
}

void sleepq_queue_insert(sleepq_queue_t *queue, struct thread *thread) {
	dlist_add_prev(&thread->sched.l_link, queue);
}

void sleepq_queue_remove(sleepq_queue_t *queue, struct thread *thread) {
	dlist_del(&thread->sched.l_link);
}

struct thread *sleepq_queue_peek(sleepq_queue_t *queue) {
	return dlist_entry(queue->next, struct thread, sched.l_link);
}

int sleepq_queue_empty(sleepq_queue_t *queue) {
	return dlist_empty(queue);
}
