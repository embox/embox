/**
 * @file
 * @brief Trivial scheduler based on lists
 *
 * @date Oct 10, 2011
 * @author Avdyukhin Dmitry
 * @author Anton Bulychev
 */

#include <kernel/thread/api.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>

static int wakeup_thread(struct runq *runq, struct thread *thread);
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
	int ret = 0; /* Test Data */
	assert(runq && thread);

	if (list_empty(&runq->rq)) ret = 1;
	list_add_tail(&thread->sched.l_link, &runq->rq);
	return ret;
}

int runq_stop(struct runq *runq, struct thread *thread) {
	assert(runq && thread);

	if (thread == runq->current) {
		return 1;
	}

	list_del(&thread->sched.l_link);
	return 0;
}

static int wakeup_thread(struct runq *runq, struct thread *thread) {
	int ret = 0; /* Test Data */
	assert(runq && thread);

	thread->state = thread_state_do_wake(thread->state);
	assert(thread_state_running(thread->state));

	thread->runq = runq;

	if (list_empty(&runq->rq)) ret = 1;
	list_add_tail(&thread->sched.l_link, &runq->rq);
	return ret;
}

int runq_wake(struct runq *runq, struct sleepq *sleepq, int wake_all) {
	int ret = 0; /* Test Data */
	struct thread *t;
	assert(runq && sleepq);

	if (sleepq_empty(sleepq)) {
		return 0;
	}

	if (!list_empty(&sleepq->rq)) {
		t = list_entry(sleepq->rq.next, struct thread, sched.l_link);
		list_del(&t->sched.l_link);
		ret = wakeup_thread(runq, t);
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

	return ret;
}

void runq_sleep(struct runq *runq, struct sleepq *sleepq) {
	struct thread *current;
	assert(runq && sleepq);

	current = runq->current;
	assert(current->runq == runq);

	list_add_tail(&current->sched.l_link, &sleepq->rq);

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
		list_add_tail(&prev->sched.l_link, &runq->rq);
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

void sleepq_change_priority(struct sleepq *sleepq, struct thread *thread, int new_priority) {
	assert(sleepq && thread);
	thread->priority = new_priority;
}

static void move_thread_to_another_q(struct list_head *q, struct thread *thread) {
	struct list_head *link;
	assert(q && thread);

	link = &thread->sched.l_link;
	list_del(link);
	list_add_tail(link, q);
}

void sleepq_on_suspend(struct sleepq *sleepq, struct thread *thread) {
	assert(sleepq && thread);
	move_thread_to_another_q(&sleepq->sq, thread);
}

void sleepq_on_resume(struct sleepq *sleepq, struct thread *thread) {
	assert(sleepq && thread);
	move_thread_to_another_q(&sleepq->rq, thread);
}

struct thread *sleepq_get_thread(struct sleepq *sleepq) {
	/**
	 * We suggest that there is only one thread in all sleepq
	 * TODO: asserts
	 */

	struct list_head *q;
	assert(sleepq && !sleepq_empty(sleepq));

	q = list_empty(&sleepq->rq) ? &sleepq->sq : &sleepq->rq;
	return list_entry(q->next, struct thread, sched.l_link);
}
