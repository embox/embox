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

#include <kernel/task.h>

#include "simple_queue.h"

void runq_init(struct runq *rq) {
	assert(rq);

	runq_queue_init(&rq->queue);
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
