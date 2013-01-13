/**
 * @file
 * @brief Fixed time scheduler
 *
 * @date Mar 6, 2012
 * @author Anton Bulychev
 */

#include <framework/mod/options.h>

#include <kernel/thread.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>

#include <stdio.h>

#define MSEC_PRIORITY_MULTIPLIER OPTION_GET(NUMBER, msec_priority_multiplier)

inline void sched_strategy_init(struct sched_strategy_data *data) {
	dlist_head_init(&data->l_link);
}

static void sched_tick(sys_timer_t *timer, void *param) {
	sched_request_switch();
}

static inline uint32_t thread_timeslice(struct thread *thread) {
	return MSEC_PRIORITY_MULTIPLIER * thread->priority;
}

void runq_init(struct runq *runq, struct thread *current, struct thread *idle) {
	assert(current && idle);

	dlist_init(&runq->rq);

	current->runq = runq;
	current->state = thread_state_do_activate(current->state);
	runq->current = current;

	runq_start(runq, idle);

	/* Initializing tick timer. */
	/* TODO: Error if not set timer and timer close. */
	if (timer_set(&runq->tick_timer, TIMER_PERIODIC,
			thread_timeslice(current), sched_tick, NULL)) {
		printf("Scheduler initialization failed!\n");
	}
}

void runq_fini(struct runq *runq) {
	timer_close(runq->tick_timer);
}

inline struct thread *runq_current(struct runq *runq) {
	return runq->current;
}

int runq_start(struct runq *runq, struct thread *thread) {
	assert(runq && thread);
	assert(runq->current != thread);
	assert(!thread_state_started(thread->state));

	thread->runq = runq;
	thread->state = thread_state_do_activate(thread->state);

	dlist_add_prev(&thread->sched.l_link, &runq->rq);

	return 0;
}

int runq_finish(struct runq *runq, struct thread *thread) {
	int is_current;
	assert(runq && thread);
	assert(thread_state_running(thread->state));

	thread->runq = NULL;
	thread->state = thread_state_do_exit(thread->state);
	if (!(is_current = (thread == runq->current))) {
		dlist_del(&thread->sched.l_link);
	}

	return is_current;
}

int sleepq_wake_thread(struct runq *runq, struct sleepq *sleepq, struct thread *thread) {
	assert(thread && runq && sleepq);
	assert(thread_state_sleeping(thread->state));

	dlist_del(&thread->sched.l_link);
	thread->state = thread_state_do_wake(thread->state);
	thread->runq = runq;
	if (thread != runq->current) {
		dlist_add_prev(&thread->sched.l_link, &runq->rq);
	}

	return 0;
}

int sleepq_wake(struct runq *runq, struct sleepq *sleepq, int wake_all) {
	struct thread *thread;
	assert(runq && sleepq);

	if (sleepq_empty(sleepq)) {
		return 0;
	}

	thread = dlist_entry(sleepq->sq.next, struct thread, sched.l_link);
	sleepq_wake_thread(runq, sleepq, thread);

	if (wake_all) {
		while (!dlist_empty(&sleepq->sq)) {
			thread = dlist_entry(sleepq->sq.next, struct thread, sched.l_link);
			sleepq_wake_thread(runq, sleepq, thread);
		}
	}

	return 0;
}

void sleepq_finish(struct sleepq *sleepq, struct thread *thread) {
    assert(thread_state_sleeping(thread->state));

    dlist_del(&thread->sched.l_link);
    thread->state = thread_state_do_exit(thread->state);
    thread->sleepq = NULL;
}

void runq_sleep(struct runq *runq, struct sleepq *sleepq) {
	struct thread *current;

	assert(runq && sleepq);

	current = runq->current;
	assert(current->runq == runq);

	dlist_add_prev(&current->sched.l_link, &sleepq->sq);

	current->sleepq = sleepq;
	current->state = thread_state_do_sleep(current->state);
}

int runq_change_priority(struct runq *runq, struct thread *thread, int new_priority) {
	assert(runq && thread);
	thread->priority = new_priority;
	return 0;
}

void sleepq_change_priority(struct sleepq *sleepq, struct thread *thread, int new_priority) {
	assert(sleepq && thread);
	thread->priority = new_priority;
}

int runq_switch(struct runq *runq) {
	struct thread *prev, *next;
	assert(runq);

	timer_close(runq->tick_timer);

	prev = runq->current;
	if (thread_state_running(prev->state)) {
		dlist_add_prev(&prev->sched.l_link, &runq->rq);
	}

	next = dlist_entry(runq->rq.next, struct thread, sched.l_link);
	dlist_del(&next->sched.l_link);

	assert(next);
	assert(thread_state_running(next->state));

	runq->current = next;

	if (thread_timeslice(next)) {
		timer_set(&runq->tick_timer, TIMER_ONESHOT,
			thread_timeslice(next), sched_tick, NULL);
	}

	return prev != next;
}

inline void sleepq_init(struct sleepq *sleepq) {
	dlist_init(&sleepq->sq);
	startq_init_sleepq(&sleepq->startq_data);
}

int sleepq_empty(struct sleepq *sleepq) {
	return dlist_empty(&sleepq->sq);
}

struct thread *sleepq_get_thread(struct sleepq *sleepq) {
	assert(sleepq && !sleepq_empty(sleepq));

	return dlist_entry(sleepq->sq.next, struct thread, sched.l_link);
}
