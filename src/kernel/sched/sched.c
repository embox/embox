/**
 * @file
 * @brief TODO --Alina
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 *          - Initial implementation
 * @author Kirill Skorodumov
 *          - Some insignificant contribution
 * @author Alina Kramar
 *          - Initial work on priority inheritance support
 * @author Eldar Abusalimov
 *          - Rewriting from scratch:
 *              - Interrupts safety, adaptation to Critical API
 *              - @c startq for deferred wake/resume processing
 */

#include <embox/unit.h>

#include <assert.h>
#include <errno.h>
#include <sched.h>
#include <time.h>

#include <hal/context.h>
#include <hal/ipl.h>

#include <kernel/critical.h>
#include <kernel/sched.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/thread.h>
#include <kernel/thread/current.h>
#include <kernel/thread/signal.h>
#include <kernel/irq_lock.h>

#include <profiler/tracing/trace.h>

#include <embox/unit.h>

#include <kernel/sched.h>


static void sched_switch(void);

CRITICAL_DISPATCHER_DEF(sched_critical, sched_switch, CRITICAL_SCHED_LOCK);

//TODO these variable for scheduler (may be create object scheduler?)
static struct runq rq;
static int switch_posted;


static inline int in_harder_critical(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

static inline int in_sched_locked(void) {
	return !critical_allows(CRITICAL_SCHED_LOCK);
}

static inline int is_running(struct thread *t) {
	return t->state & __THREAD_STATE_RUNNING;
}

static inline int is_ready(struct thread *t) {
	return t->state & __THREAD_STATE_READY;
}

static inline int is_waiting(struct thread *t) {
	return t->state & __THREAD_STATE_WAITING;
}

static inline void make_ready(struct thread *t) {
	t->state |= __THREAD_STATE_READY;
	t->state &= ~__THREAD_STATE_RUNNING;
}

static inline void make_running(struct thread *t) {
	t->state |= __THREAD_STATE_RUNNING;
	t->state &= ~__THREAD_STATE_READY;
}

int sched_init(struct thread *idle, struct thread *current) {
	assert(idle && current);

	runq_init(&rq.queue);

	sched_wake(idle);

	sched_ticker_init();

	return 0;
}

void sched_wake(struct thread *t) {
	struct thread *current = thread_self();

	/* TODO: it's not true when it's called for waking thread from waitq */
	//assert(!in_harder_critical());
	assert(t != current);
	assert(t);
	assert(is_waiting(t));

	irq_lock();
	{

		t->state &= ~__THREAD_STATE_WAITING;

		if (!is_ready(t)) {
			make_ready(t);
			runq_insert(&rq.queue, t);
		}

		if (thread_priority_get(t) > thread_priority_get(current)) {
			sched_post_switch();
		}
	}
	irq_unlock();
}

void sched_finish(struct thread *t) {
	assert(!in_harder_critical());
	assert(t);

	irq_lock();
	{
		if (is_running(t)) {
			assert(t == thread_self(), "XXX SMP NIY");
			sched_post_switch();
		}

		if (is_ready(t)) {
			runq_remove(&rq.queue, t);
		}

		if (is_waiting(t)) {
			waitq_remove(t->wait_link);
		}

		t->state = __THREAD_STATE_DO_EXITED(t->state);
	}
	irq_unlock();
}

void sched_post_switch(void) {
	irq_lock();
	{
		switch_posted = 1;
		critical_request_dispatch(&sched_critical);
	}
	irq_unlock();
}

int sched_change_priority(struct thread *t, sched_priority_t prior) {
	struct thread *current = thread_self();

	assert(t);
	assert((prior >= SCHED_PRIORITY_MIN) && (prior <= SCHED_PRIORITY_MAX));

	irq_lock();
	{
		assert(!__THREAD_STATE_IS_EXITED(t->state));

		thread_priority_set(t, prior);

		if (is_ready(t)) {
			runq_remove(&rq.queue, t);
			runq_insert(&rq.queue, t);

			if (prior > thread_priority_get(current)) {
				sched_post_switch();
			}
		}

		assert(thread_priority_get(t) == prior);
	}
	irq_unlock();

	return 0;
}

/**
 * Called by critical dispatching code with IRQs disabled.
 */
static void sched_switch(void) {
	struct thread *prev, *next;

	assert(!in_sched_locked());

	sched_lock();
	{
		if (!switch_posted) {
			goto out;
		}
		switch_posted = 0;

		prev = thread_get_current();

		if (is_running(prev)) {
			make_ready(prev);
			runq_insert(&rq.queue, prev);
		}

		next = runq_extract(&rq.queue);

		assert(next != NULL);
		assert(is_ready(next));

		make_running(next);

		if (prev == next) {
			goto out;
		}

		sched_ticker_switch(prev, next);
		sched_timing_switch(prev, next);

		trace_point("context switch");

		thread_set_current(next);
		context_switch(&prev->context, &next->context);
	}
out:
	sched_unlock_noswitch();

	thread_signal_handle();
}


