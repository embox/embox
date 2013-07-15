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
#include <kernel/sched.h>
#include <kernel/sched/sched_strategy.h>
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
	current->state = thread_state_do_oncpu(current->state);

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
	current->state = thread_state_do_oncpu(current->state);
}

void runq_fini(struct runq *rq) {
	timer_close(rq->tick_timer);
}

static void sched_tick(sys_timer_t *timer, void *param) {
	extern void smp_send_resched(int cpu_id);
	sched_post_switch();

#ifdef SMP
	for (int i = 0; i < NCPU; i++) {
		smp_send_resched(i);
	}
#endif
}

int runq_start(struct runq *rq, struct thread *t) {
	struct thread *current = thread_self();

	assert(rq);
	assert(t);
	assert(current != t);
	assert(!thread_state_started(t->state));

	t->runq = rq;
	t->state = thread_state_do_activate(t->state);

	runq_queue_insert(&rq->queue, t);

	return thread_priority_get(t) > thread_priority_get(current);
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

int runq_wake_thread(struct runq *rq, struct thread *t) {
	struct thread *current = thread_self();

	assert(t && rq);
	assert(thread_state_sleeping(t->state));

	t->state = thread_state_do_wake(t->state);
	t->runq = rq;
	if (t != current) {
		runq_queue_insert(&rq->queue, t);
	}

	return thread_priority_get(t) > thread_priority_get(current);
}

void runq_wait(struct runq *rq) {
	struct thread *current = thread_self();

	assert(rq);
	assert(current->runq == rq);

	current->state = thread_state_do_sleep(current->state);
}

int runq_change_priority(struct runq *rq, struct thread *t, sched_priority_t new_priority) {
	struct thread *current = thread_self();

	assert(rq);
	assert(t);

	thread_priority_set(t, new_priority);

	if (current != t) {
		/* FIXME: */
		runq_queue_remove(&rq->queue, t);
		runq_queue_insert(&rq->queue, t);
	}

	return new_priority > thread_priority_get(current);
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

	if (next != current) {
		current->state = thread_state_do_outcpu(current->state);
		next->state = thread_state_do_oncpu(next->state);
	}

	return next;
}
