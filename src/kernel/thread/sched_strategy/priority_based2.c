/**
 * @file
 * @brief Preemptive priority scheduler based on multilevel queue.
 *
 * @date 05.03.2013
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

#include "priority_based2.h"

/* Queue operations */
static inline void queue_init(struct prior_queue *queue) {
	for (int i = THREAD_PRIORITY_MIN; i <= THREAD_PRIORITY_MAX; i++) {
		dlist_init(&queue->list[i]);
	}
}

static inline void queue_insert(struct prior_queue *queue, struct thread *thread) {
	dlist_add_prev(&thread->sched.link, &queue->list[thread->priority]);
}

static inline void queue_remove(struct prior_queue *queue, struct thread *thread) {
	dlist_del(&thread->sched.link);
}

static inline struct thread *queue_peek(struct prior_queue *queue) {
	for (int i = THREAD_PRIORITY_MAX; i >= THREAD_PRIORITY_MIN; i--) {
		if (!dlist_empty(&queue->list[i])) {
			return dlist_entry(queue->list[i].next, struct thread, sched.link);
		}
	}

	return NULL;
}

static inline struct thread *queue_extract(struct prior_queue *queue) {
	struct thread *thread = queue_peek(queue);

	if (thread) {
		queue_remove(queue, thread);
	}

	return thread;
}

static inline int queue_empty(struct prior_queue *queue) {
	return queue_peek(queue) == NULL;
}


static void sched_tick(sys_timer_t *timer, void *param);

static void change_thread_priority(struct prior_queue *pq, struct thread *t,
		int new_priority);

void runq_init(struct runq *rq, struct thread *current, struct thread *idle) {
	assert(current && idle);
	queue_init(&rq->queue);

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

	queue_insert(&rq->queue, t);

	return (t->priority > current->priority);
}

int runq_finish(struct runq *rq, struct thread *t) {
	int is_current;
	assert(rq && t);
	assert(thread_state_running(t->state));

	t->runq = NULL;
	t->state = thread_state_do_exit(t->state);
	if (!(is_current = (t == thread_self()))) {
		queue_remove(&rq->queue, t);
	}

	return is_current;
}

int sleepq_wake_thread(struct runq *rq, struct sleepq *sq, struct thread *t) {
	struct thread *current = thread_self();

	assert(t && rq && sq);
	assert(thread_state_sleeping(t->state));

	queue_remove(&sq->queue, t);
	t->state = thread_state_do_wake(t->state);
	t->runq = rq;
	if (t != current) {
		queue_insert(&rq->queue, t);
	}

	return (t->priority > current->priority);
}

int sleepq_wake(struct runq *rq, struct sleepq *sq, int wake_all) {
	int ret = 0;
	struct thread *t;

	assert(rq && sq);

	if (sleepq_empty(sq)) {
		return 0;
	}

	if ((t = queue_peek(&sq->queue))) {
		ret = sleepq_wake_thread(rq, sq, t);
	}

	if (wake_all) {
		while ((t = queue_peek(&sq->queue))) {
			sleepq_wake_thread(rq, sq, t);
		}
	}

	return ret;
}

void sleepq_finish(struct sleepq *sq, struct thread *t) {
    assert(thread_state_sleeping(t->state));

    queue_remove(&sq->queue, t);
    t->state = thread_state_do_exit(t->state);
    t->sleepq = NULL;
}

void runq_sleep(struct runq *rq, struct sleepq *sq) {
	struct thread *current = thread_self();

	assert(rq && sq);
	assert(current->runq == rq);

	queue_insert(&sq->queue, current);

	current->sleepq = sq;
	current->state = thread_state_do_sleep(current->state);
}

int runq_change_priority(struct runq *rq, struct thread *t, int new_priority) {
	struct thread *current = thread_self();

	assert(rq && t);

	if (current == t) {
		t->priority = new_priority;
	} else {
		change_thread_priority(&rq->queue, t, new_priority);
	}

	return (new_priority > current->priority);
}

struct thread *runq_switch(struct runq *rq) {
	struct thread *current = thread_self();
	struct thread *next;

	assert(rq);

	if (thread_state_running(current->state)) {
		queue_insert(&rq->queue, current);
	}

	next = queue_extract(&rq->queue);

	assert(next != NULL);
	assert(thread_state_running(next->state));

	return next;
}

void sched_strategy_init(struct sched_strategy_data *s) {
	dlist_head_init(&s->link);
}

void sleepq_init(struct sleepq *sq) {
	queue_init(&sq->queue);
	startq_init_sleepq(&sq->startq_data);
}

int sleepq_empty(struct sleepq *sq) {
	assert(sq);
	return queue_empty(&sq->queue);
}

void sleepq_change_priority(struct sleepq *sq, struct thread *t,
		int new_priority) {
	assert(sq && t);
	change_thread_priority(&sq->queue, t, new_priority);
}

static void change_thread_priority(struct prior_queue *pq, struct thread *t,
		int new_priority) {

	assert(pq && t);

	queue_remove(pq, t);
	t->priority = new_priority;
	queue_insert(pq, t);
}

struct thread *sleepq_get_thread(struct sleepq *sq) {
	assert(sq && !sleepq_empty(sq));
	return queue_peek(&sq->queue);
}
