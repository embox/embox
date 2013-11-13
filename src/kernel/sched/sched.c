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

#include <assert.h>
#include <errno.h>
#include <time.h>

#include <kernel/critical.h>
#include <kernel/thread.h>
#include <kernel/thread/current.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/thread/state.h>
#include <kernel/task/signal.h>

#include <hal/context.h>
#include <hal/ipl.h>

#include <profiler/tracing/trace.h>

#include <embox/unit.h>

#include <kernel/sched.h>

#include <sched.h>





static void sched_switch(void);


CRITICAL_DISPATCHER_DEF(sched_critical, sched_switch, CRITICAL_SCHED_LOCK);

//TODO these variable for scheduler (may be create object scheduler?)
struct runq rq;
static int switch_posted;


static inline int in_harder_critical(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

static inline int in_sched_locked(void) {
	return !critical_allows(CRITICAL_SCHED_LOCK);
}

int sched_init(struct thread *idle, struct thread *current) {
	assert(idle && current);

	runq_queue_init(&rq.queue);

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
	assert(t->state & __THREAD_STATE_WAITING);

	sched_lock();
	{
		t->state |= __THREAD_STATE_READY;
		t->state &= ~(__THREAD_STATE_WAITING | __THREAD_STATE_RUNNING);
		runq_queue_insert(&rq.queue, t);

		if (thread_priority_get(t) > thread_priority_get(current)) {
			sched_post_switch();
		}
	}
	sched_unlock();
}

void sched_finish(struct thread *t) {
	assert(!in_harder_critical());
	assert(t);

	sched_lock();
	{
		assert(!__THREAD_STATE_IS_EXITED(t->state));

		if (t->state & (__THREAD_STATE_READY | __THREAD_STATE_RUNNING)) {
			t->state = __THREAD_STATE_DO_EXITED(t->state);

			if (t != thread_self()) {
				runq_queue_remove(&rq.queue, t);
			} else {
				sched_post_switch();
			}
		} else {
			if (t->state & __THREAD_STATE_WAITING) {
				waitq_remove(t->wait_link);
			}

			t->state = __THREAD_STATE_DO_EXITED(t->state);
		}

		assert(__THREAD_STATE_IS_EXITED(t->state));
	}
	sched_unlock();
}

void sched_post_switch(void) {
	sched_lock();
	{
		switch_posted = 1;
		critical_request_dispatch(&sched_critical);
	}
	sched_unlock();
}

int sched_change_priority(struct thread *t, sched_priority_t prior) {
	struct thread *current = thread_self();

	assert(t);
	assert((prior >= SCHED_PRIORITY_MIN) && (prior <= SCHED_PRIORITY_MAX));

	sched_lock();
	{
		assert(!__THREAD_STATE_IS_EXITED(t->state));

		thread_priority_set(t, prior);

		/* if in runq */
		if (t->state & __THREAD_STATE_READY) {
			runq_queue_remove(&rq.queue, t);
			runq_queue_insert(&rq.queue, t);

			if (prior > thread_priority_get(current)) {
				sched_post_switch();
			}
		}

		assert(thread_priority_get(t) == prior);
	}
	sched_unlock();

	return 0;
}

/**
 * Called by critical dispatching code with IRQs disabled.
 */
static void sched_switch(void) {
	struct thread *prev, *next;
	clock_t new_clock;

	assert(!in_sched_locked());

	sched_lock();
	{
		if (!switch_posted) {
			goto out;
		}
		switch_posted = 0;

		ipl_enable();

		prev = thread_get_current();

		if (prev->state & __THREAD_STATE_RUNNING) {
			runq_queue_insert(&rq.queue, prev);
		}

		next = runq_queue_extract(&rq.queue);

		assert(next != NULL);
		assert(next->state & (__THREAD_STATE_RUNNING | __THREAD_STATE_READY));

		if (prev == next) {
			ipl_disable();
			goto out;
		} else {
			if (prev->state & __THREAD_STATE_RUNNING) {
				prev->state |= __THREAD_STATE_READY;
				prev->state &= ~(__THREAD_STATE_RUNNING | __THREAD_STATE_WAITING);
			}
			next->state |= __THREAD_STATE_RUNNING;
			next->state &= ~(__THREAD_STATE_READY | __THREAD_STATE_WAITING);
		}

		if (prev->policy == SCHED_FIFO && next->policy != SCHED_FIFO) {
			sched_ticker_init();
		}

		if (prev->policy != SCHED_FIFO && next->policy == SCHED_FIFO) {
			sched_ticker_fini(&rq);
		}

		/* Running time recalculation */
		new_clock = clock();
		sched_timing_stop(prev, new_clock);
		sched_timing_start(next, new_clock);

		trace_point("context switch");

		ipl_disable();

		thread_set_current(next);
		context_switch(&prev->context, &next->context);
	}
out:
	sched_unlock_noswitch();

	task_signal_hnd();
}


