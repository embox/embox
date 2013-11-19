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

int sched_init(struct thread *idle, struct thread *current) {
	assert(idle && current);

	runq_init(&rq.queue);

	sched_wake(idle);

	sched_ticker_init();

	return 0;
}

void sched_wake(struct thread *t) {
	struct thread *current = thread_self();
	ipl_t ipl;

	/* TODO: it's not true when it's called for waking thread from waitq */
	//assert(!in_harder_critical());
	assert(t != current);
	assert(t);
	assert(t->state & __THREAD_STATE_WAITING);

	ipl = ipl_save();
	{
		t->state |= __THREAD_STATE_READY;
		t->state &= ~(__THREAD_STATE_WAITING | __THREAD_STATE_RUNNING);
		runq_insert(&rq.queue, t);

		if (thread_priority_get(t) > thread_priority_get(current)) {
			sched_post_switch();
		}
	}
	ipl_restore(ipl);
}

void sched_finish(struct thread *t) {
	ipl_t ipl;

	assert(!in_harder_critical());
	assert(t);

	ipl = ipl_save();
	{
		if (t->state & __THREAD_STATE_RUNNING) {
			assert(t == thread_self(), "XXX SMP NIY");
			sched_post_switch();
		}

		if (t->state & __THREAD_STATE_READY) {
			runq_remove(&rq.queue, t);
		}

		if (t->state & __THREAD_STATE_WAITING) {
			waitq_remove(t->wait_link);
		}

		t->state = __THREAD_STATE_DO_EXITED(t->state);
	}
	ipl_restore(ipl);
}

void sched_post_switch(void) {
	ipl_t ipl = ipl_save();
	{
		switch_posted = 1;
		critical_request_dispatch(&sched_critical);
	}
	ipl_restore(ipl);
}

int sched_change_priority(struct thread *t, sched_priority_t prior) {
	struct thread *current = thread_self();
	ipl_t ipl;

	assert(t);
	assert((prior >= SCHED_PRIORITY_MIN) && (prior <= SCHED_PRIORITY_MAX));

	ipl = ipl_save();
	{
		assert(!__THREAD_STATE_IS_EXITED(t->state));

		thread_priority_set(t, prior);

		if (t->state & __THREAD_STATE_READY) {
			runq_remove(&rq.queue, t);
			runq_insert(&rq.queue, t);

			if (prior > thread_priority_get(current)) {
				sched_post_switch();
			}
		}

		assert(thread_priority_get(t) == prior);
	}
	ipl_restore(ipl);

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

		ipl_enable();

		prev = thread_get_current();

		if (prev->state & __THREAD_STATE_RUNNING) {
			prev->state |= __THREAD_STATE_READY;
			prev->state &= ~(__THREAD_STATE_RUNNING | __THREAD_STATE_WAITING);
			runq_insert(&rq.queue, prev);
		}

		next = runq_extract(&rq.queue);

		assert(next != NULL);
		assert(next->state & (__THREAD_STATE_RUNNING | __THREAD_STATE_READY));

		next->state |= __THREAD_STATE_RUNNING;
		next->state &= ~(__THREAD_STATE_READY | __THREAD_STATE_WAITING);

		if (prev == next) {
			ipl_disable();
			goto out;
		}

		sched_ticker_switch(prev, next);
		sched_timing_switch(prev, next);

		trace_point("context switch");

		ipl_disable();

		thread_set_current(next);
		context_switch(&prev->context, &next->context);
	}
out:
	sched_unlock_noswitch();

	task_signal_hnd();
}


