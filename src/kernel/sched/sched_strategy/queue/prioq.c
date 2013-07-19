/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <util/prioq.h>

#include <kernel/thread.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/task.h>

/* return -1 if priority t1 less than t2
 * 0 if them equals
 * 1 if priority t1 more than t2
 */
static inline int thread_prio_comparator(struct prioq_link *first,
		struct prioq_link *second) {
	struct thread *t1 = prioq_element(first, struct thread, sched.pq_link);
	struct thread *t2 = prioq_element(second, struct thread, sched.pq_link);
	__thread_priority_t p1, p2;

	p1 = thread_priority_get(t1);
	p2 = thread_priority_get(t2);

	return p1 < p2 ? (-1) : (p1 > p2);
}

void sched_strategy_init(struct sched_strategy_data *s) {
	prioq_link_init(&s->pq_link);
}

void runq_queue_init(runq_queue_t *queue) {
	prioq_init(queue);
}

void runq_queue_insert(runq_queue_t *queue, struct thread *thread) {
	prioq_enqueue(thread, thread_prio_comparator, queue, sched.pq_link);
}

void runq_queue_remove(runq_queue_t *queue, struct thread *thread) {
	prioq_remove(thread, thread_prio_comparator, sched.pq_link);
}

struct thread *runq_queue_extract(runq_queue_t *queue) {
	return prioq_dequeue(thread_prio_comparator, queue, struct thread,
			sched.pq_link);
}

void sleepq_queue_init(sleepq_queue_t *queue) {
	prioq_init(queue);
}

void sleepq_queue_insert(sleepq_queue_t *queue, struct thread *thread) {
	prioq_enqueue(thread, thread_prio_comparator, queue, sched.pq_link);
}

void sleepq_queue_remove(sleepq_queue_t *queue, struct thread *thread) {
	prioq_remove(thread, thread_prio_comparator, sched.pq_link);
}

struct thread *sleepq_queue_peek(sleepq_queue_t *queue) {
	return prioq_peek(thread_prio_comparator, queue, struct thread,
			sched.pq_link);
}

int sleepq_queue_empty(sleepq_queue_t *queue) {
	return prioq_empty(queue);
}
