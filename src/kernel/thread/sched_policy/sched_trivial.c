/**
 * @file
 * @brief Trivial scheduler based on lists
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 * @author Anton Bulychev
 */

#include <kernel/thread/api.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>

static void wakeup_thread(struct runq *runq, struct thread *thread);
static void move_thread_to_another_q(struct list_head *q, struct thread *thread);

inline void sched_strategy_init(struct sched_strategy_data *data) {
	INIT_LIST_HEAD(&data->l_link);
}

void runq_init(struct runq *runq, struct thread *current, struct thread *idle) {
	INIT_LIST_HEAD(&runq->rq);
	runq->current = current;
	runq_start(runq, idle);
}

inline struct thread *runq_current(struct runq *runq) {
	assert(runq);

	return runq->current;
}

int runq_start(struct runq *runq, struct thread *thread) {
	assert(runq && thread);

	list_add_tail(&runq->rq, &thread->sched.l_link);
	return 0;
}

int runq_stop(struct runq *runq, struct thread *thread) {
	assert(runq && thread);

	if (thread == runq->current) {
		return 0;
	}

	list_del(&thread->sched.l_link);
	return 1;
}

static void wakeup_thread(struct runq *runq, struct thread *thread) {
	assert(runq && thread);

	thread->state = thread_state_do_wake(thread->state);
	assert(thread_state_running(thread->state));

	thread->runq = runq;

	list_add_tail(&runq->rq, &thread->sched.l_link);
}

int runq_wake(struct runq *runq, struct sleepq *sleepq, int wake_all) {
	struct thread *t;
	assert(runq && sleepq);

	if (sleepq_empty(sleepq)) {
		return 0;
	}

	if (!list_empty(&sleepq->rq)) {
		t = list_entry(sleepq->rq.next, struct thread, sched.l_link);
		list_del(&t->sched.l_link);
		wakeup_thread(runq, t);
	} else {
		assert(!list_empty(&sleepq->sq));
		t = list_entry(sleepq->sq.next, struct thread, sched.l_link);
		list_del(&t->sched.l_link);

		t->state = thread_state_do_wake(t->state);
		assert(thread_state_suspended(t->state));
	}


	if (wake_all) {
		while (!list_empty(&sleepq->rq)) {
			t = list_entry(sleepq->rq.next, struct thread, sched.l_link);
			list_del(&t->sched.l_link);
			wakeup_thread(runq, t);
		}
		while (!list_empty(&sleepq->sq)) {
			t = list_entry(sleepq->sq.next, struct thread, sched.l_link);
			list_del(&t->sched.l_link);

			t->state = thread_state_do_wake(t->state);
			assert(thread_state_suspended(t->state));
		}
	}

	return 0;
}

void runq_sleep(struct runq *runq, struct sleepq *sleepq) {
	struct thread *current;
	assert(runq && sleepq);

	current = runq->current;
	assert(current->runq == runq);

	list_add_tail(&sleepq->rq, &current->sched.l_link);

	current->sleepq = sleepq;
	current->state = thread_state_do_sleep(current->state);
}

int runq_change_priority(struct runq *runq, struct thread *thread, int new_priority) {
	assert(runq && thread);

	thread->priority = new_priority;
	return 0;
}

int runq_switch(struct runq *runq) {
	struct thread *prev, *next;
	assert(runq);

	prev = runq->current;
	if (thread_state_running(prev->state)) {
		list_add_tail(&runq->rq, &prev->sched.l_link);
	}

	assert(!list_empty(&runq->rq));

	next = list_entry(runq->rq.next, struct thread, sched.l_link);
	list_del(&next->sched.l_link);

	assert(next);
	assert(thread_state_running(next->state));

	runq->current = next;
	return prev != next;
}

inline void sleepq_init(struct sleepq *sleepq) {
	INIT_LIST_HEAD(&sleepq->rq);
	INIT_LIST_HEAD(&sleepq->sq);
}

int sleepq_empty(struct sleepq *sleepq) {
	assert(sleepq);

	return list_empty(&sleepq->rq) && list_empty(&sleepq->sq);
}

#if 0
#define sleepq_foreach(thread, sleepq) \
	  __sleepq_foreach(thread, sleepq)

#define __sleepq_foreach(t, sleepq) \
	for(t = NULL;;)
#endif

void sleepq_change_priority(struct sleepq *sleepq, struct thread *thread, int new_priority) {
	assert(sleepq && thread);
	thread->priority = new_priority;
}

static void move_thread_to_another_q(struct list_head *q, struct thread *thread) {
	struct list_head *link;

	assert(q && thread);

	link = &thread->sched.l_link;
	list_del(link);
	list_add_tail(q, link);
}

void sleepq_on_suspend(struct sleepq *sleepq, struct thread *thread) {
	assert(sleepq && thread);

	move_thread_to_another_q(&sleepq->sq, thread);
}

void sleepq_on_resume(struct sleepq *sleepq, struct thread *thread) {
	assert(sleepq && thread);

	move_thread_to_another_q(&sleepq->rq, thread);
}
