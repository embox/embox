/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <framework/mod/options.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include <kernel/thread.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>

#include <kernel/cpu.h>
#include <kernel/time/timer.h>

#include <kernel/task.h>

#include "simple_queue.h"

static void sched_tick(sys_timer_t *timer, void *param);

void runq_init(struct runq *rq, struct thread *current, struct thread *idle) {
	assert(current && idle);
	runq_queue_init(&rq->queue);

	current->runq = rq;
	idle->runq = rq;

	current->state = thread_state_do_activate(current->state);

	runq_start(rq, idle);

	/* Initializing tick timer. */
	/* TODO: Error if not set timer and timer close. */
	if (timer_set(&rq->tick_timer, TIMER_PERIODIC,
			OPTION_GET(NUMBER, tick_interval), sched_tick, NULL)) {
		printf("Scheduler initialization failed!\n");
	}
}

void runq_cpu_init(struct runq *rq, struct thread *current) {
	assert(rq && current);

	current->runq = rq;
	current->state = thread_state_do_activate(current->state);
}

void runq_fini(struct runq *rq) {
	timer_close(rq->tick_timer);
}

static void sched_tick(sys_timer_t *timer, void *param) {
	extern void smp_send_resched(int cpu_id);
	sched_post_switch();

#ifdef SMP
	smp_send_resched(1);
#endif
}

int runq_start(struct runq *rq, struct thread *t) {
	struct thread *current = thread_self();

	assert(rq && t);
	assert(thread_self() != t);
	assert(!thread_state_started(t->state));

	t->runq = rq;
	t->state = thread_state_do_activate(t->state);

	runq_queue_insert(&rq->queue, t);

	return (t->task->priority > current->task->priority)
			|| ((t->task->priority == current->task->priority)
				&& (t->priority > current->priority));
}

int runq_finish(struct runq *rq, struct thread *t) {
	int is_current;
	assert(rq && t);
	assert(thread_state_running(t->state));

	t->runq = NULL;
	t->state = thread_state_do_exit(t->state);
	if (!(is_current = (t == thread_self()))) {
		runq_queue_remove(&rq->queue, t);
	}

	return is_current;
}

void sleepq_init(struct sleepq *sq) {
	sleepq_queue_init(&sq->queue);
	startq_init_sleepq(&sq->startq_data);
}

int sleepq_wake_thread(struct runq *rq, struct sleepq *sq, struct thread *t) {
	struct thread *current = thread_self();

	assert(t && rq && sq);
	assert(thread_state_sleeping(t->state));

	sleepq_queue_remove(&sq->queue, t);
	t->state = thread_state_do_wake(t->state);
	t->runq = rq;
	if (t != current) {
		runq_queue_insert(&rq->queue, t);
	}

	return (t->task->priority > current->task->priority)
			|| ((t->task->priority == current->task->priority)
				&& (t->priority > current->priority));
}

int sleepq_wake(struct runq *rq, struct sleepq *sq, int wake_all) {
	int ret = 0;
	struct thread *t;

	assert(rq && sq);

	if (sleepq_empty(sq)) {
		return 0;
	}

	if ((t = sleepq_queue_peek(&sq->queue))) {
		ret = sleepq_wake_thread(rq, sq, t);
	}

	if (wake_all) {
		while ((t = sleepq_queue_peek(&sq->queue))) {
			sleepq_wake_thread(rq, sq, t);
		}
	}

	return ret;
}

void sleepq_finish(struct sleepq *sq, struct thread *t) {
    assert(thread_state_sleeping(t->state));

    sleepq_queue_remove(&sq->queue, t);
    t->state = thread_state_do_exit(t->state);
    t->sleepq = NULL;
}

void runq_sleep(struct runq *rq, struct sleepq *sq) {
	struct thread *current = thread_self();

	assert(rq && sq);
	assert(current->runq == rq);

	sleepq_queue_insert(&sq->queue, current);

	current->sleepq = sq;
	current->state = thread_state_do_sleep(current->state);
}

int runq_change_priority(struct runq *rq, struct thread *t, int new_priority) {
	struct thread *current = thread_self();

	assert(rq && t);

	runq_queue_remove(&rq->queue, t);
	t->priority = new_priority;
	runq_queue_insert(&rq->queue, t);

	return (t->task->priority > current->task->priority)
			|| ((t->task->priority == current->task->priority)
				&& (t->priority > current->priority));
}

struct thread *runq_switch(struct runq *rq) {
	struct thread *current = thread_self();
	struct thread *next;

	assert(rq);

	if (thread_state_running(current->state)) {
		runq_queue_insert(&rq->queue, current);
	}

	next = runq_queue_extract(&rq->queue);

	assert(next != NULL);
	assert(thread_state_running(next->state));

	return next;
}

int sleepq_empty(struct sleepq *sq) {
	assert(sq);
	return sleepq_queue_empty(&sq->queue);
}

void sleepq_change_priority(struct sleepq *sq, struct thread *t,
		int new_priority) {
	assert(sq && t);

	/* FIXME: */
	sleepq_queue_remove(&sq->queue, t);
	t->priority = new_priority;
	sleepq_queue_insert(&sq->queue, t);
}

struct thread *sleepq_get_thread(struct sleepq *sq) {
	assert(sq && !sleepq_empty(sq));
	return sleepq_queue_peek(&sq->queue);
}
