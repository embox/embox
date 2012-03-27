/**
 * @file
 * @brief Preemptive priority scheduler based on multilevel queue.
 *
 * @date 22.04.10
 * @author Darya Dzendzik
 *          - Initial implementation
 * @author Alina Kramar
 *          - Fixing switch logic
 *          - Interface change
 *          - Introducing auxiliary functions for operating with queue
 *
 * @see tests/kernel/thread/sched_policy/priority_based_test.c
 */

#include <assert.h>
#include <stddef.h>

#include <lib/list.h>
#include <util/array.h>
#include <util/prioq.h>

#include <kernel/thread/api.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>

static void move_thread_to_another_q(struct prioq *pq, struct thread *t);
static void change_thread_priority(struct prioq *pq, struct thread *t,
		int new_priority);

static inline int thread_prio_comparator(struct prioq_link *first,
		struct prioq_link *second) {
	struct thread *t1 = prioq_element(first, struct thread, sched.pq_link);
	struct thread *t2 = prioq_element(second, struct thread, sched.pq_link);
	return t1->priority - t2->priority;
}

void runq_init(struct runq *rq, struct thread *current, struct thread *idle) {
	assert(current && idle);
	prioq_init(&rq->pq);

	current->runq = rq;
	idle->runq = rq;

	current->state = thread_state_do_resume(current->state);
	rq->current = current;

	runq_resume(rq, idle);
}

int runq_resume(struct runq *rq, struct thread *t) {
	assert(rq && t);
	assert(thread_state_suspended(t->state));

	t->runq = rq;
	t->state = thread_state_do_resume(t->state);

	if (t != rq->current) {
		prioq_enqueue(t, thread_prio_comparator, &rq->pq, sched.pq_link);
	}

	return (t->priority > rq->current->priority);
}

int runq_suspend(struct runq *rq, struct thread *t) {
	int is_current;
	assert(rq && t);
	assert(!thread_state_suspended(t->state));

	t->runq = NULL;
	t->state = thread_state_do_suspend(t->state);
	if (!(is_current = (t == rq->current))) {
		prioq_remove(t, thread_prio_comparator, sched.pq_link);
	}

	return is_current;
}

int sleepq_wake_resumed_thread(struct runq *rq, struct sleepq *sq, struct thread *t) {
	assert(t && rq && sq);
	assert(thread_state_sleeping(t->state) && !thread_state_suspended(t->state));

	prioq_remove(t, thread_prio_comparator, sched.pq_link);
	t->state = thread_state_do_wake(t->state);
	t->runq = rq;
	if (t != rq->current) {
		prioq_enqueue(t, thread_prio_comparator, &rq->pq, sched.pq_link);
	}

	return (t->priority > rq->current->priority);
}

void sleepq_wake_suspended_thread(struct sleepq *sq, struct thread *t) {
	assert(sq && t);
	assert(thread_state_sleeping(t->state) && thread_state_suspended(t->state));

	prioq_remove(t, thread_prio_comparator, sched.pq_link);
	t->state = thread_state_do_wake(t->state);
	t->sleepq = NULL;
}

int sleepq_wake(struct runq *rq, struct sleepq *sq, int wake_all) {
	int ret = 0;
	struct thread *t;

	assert(rq && sq);

	if (sleepq_empty(sq)) {
		return 0;
	}

	if ((t = prioq_peek(thread_prio_comparator, &sq->pq,
			struct thread, sched.pq_link))) {
		ret = sleepq_wake_resumed_thread(rq, sq, t);
	} else {
		t = prioq_peek(thread_prio_comparator, &sq->suspended,
				struct thread, sched.pq_link);
		sleepq_wake_suspended_thread(sq, t);
	}

	if (wake_all) {
		while ((t = prioq_peek(thread_prio_comparator, &sq->pq,
				struct thread, sched.pq_link))) {
			ret |= sleepq_wake_resumed_thread(rq, sq, t);
		}

		while ((t = prioq_peek(thread_prio_comparator, &sq->suspended,
				struct thread, sched.pq_link))) {
			sleepq_wake_suspended_thread(sq, t);
		}
	}

	return ret;
}

void runq_sleep(struct runq *rq, struct sleepq *sq) {
	struct thread *current;

	assert(rq && sq);

	current = rq->current;
	assert(current->runq == rq);

	prioq_enqueue(current, thread_prio_comparator, &sq->pq, sched.pq_link);

	current->sleepq = sq;
	current->state = thread_state_do_sleep(current->state);
}

int runq_change_priority(struct runq *rq, struct thread *t, int new_priority) {
	assert(rq && t);

	change_thread_priority(&rq->pq, t, new_priority);

	return (new_priority > rq->current->priority);
}

int runq_switch(struct runq *rq) {
	struct thread *prev, *next;

	assert(rq);

	prev = rq->current;

	if (thread_state_running(prev->state)) {
		prioq_enqueue(prev, thread_prio_comparator, &rq->pq, sched.pq_link);
	}

	next = prioq_dequeue(thread_prio_comparator, &rq->pq,
			struct thread, sched.pq_link);

	assert(next != NULL);
	assert(thread_state_running(next->state));

	rq->current = next;

	return prev != next;
}

int sleepq_empty(struct sleepq *sq) {
	assert(sq);
	return prioq_empty(&sq->pq) && prioq_empty(&sq->suspended);
}

void sleepq_change_priority(struct sleepq *sq, struct thread *t,
		int new_priority) {
	struct prioq *pq;

	assert(sq && t);

	pq = thread_state_suspended(t->state) ? &sq->suspended : &sq->pq;

	change_thread_priority(pq, t, new_priority);
}

void sleepq_suspend(struct sleepq *sq, struct thread *t) {
	assert(sq && t);
	assert(!thread_state_suspended(t->state));

	move_thread_to_another_q(&sq->suspended, t);
	t->state = thread_state_do_suspend(t->state);
}

void sleepq_resume(struct sleepq *sq, struct thread *t) {
	assert(sq && t);
	assert(thread_state_suspended(t->state));

	t->state = thread_state_do_resume(t->state);
	move_thread_to_another_q(&sq->pq, t);
}

static void move_thread_to_another_q(struct prioq *pq, struct thread *t) {
	struct prioq_link *link;

	assert(pq && t);

	link = &t->sched.pq_link;

	prioq_remove_link(link, thread_prio_comparator);
	prioq_enqueue_link(link, thread_prio_comparator, pq);
}

static void change_thread_priority(struct prioq *pq, struct thread *t,
		int new_priority) {
	struct prioq_link *link;

	assert(pq && t);

	link = &t->sched.pq_link;

	prioq_remove_link(link, thread_prio_comparator);
	t->priority = new_priority;
	prioq_enqueue_link(link, thread_prio_comparator, pq);
}

struct thread *sleepq_get_thread(struct sleepq *sq) {
	struct prioq *pq;
	assert(sq && !sleepq_empty(sq));
	pq = prioq_empty(&sq->pq) ? &sq->suspended : &sq->pq;
	return prioq_peek(thread_prio_comparator, pq, struct thread, sched.pq_link);
}
