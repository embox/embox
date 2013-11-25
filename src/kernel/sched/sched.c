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
#include <sched.h>

#include <kernel/sched.h>

#include <hal/context.h>
#include <hal/ipl.h>

#include <kernel/critical.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/thread.h>
#include <kernel/thread/current.h>
#include <kernel/thread/signal.h>
#include <kernel/irq_lock.h>

#include <profiler/tracing/trace.h>

#include <embox/unit.h>

static void sched_preempt(void);

CRITICAL_DISPATCHER_DEF(sched_critical, sched_preempt, CRITICAL_SCHED_LOCK);

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

	current->state = THREAD_READY | THREAD_ACTIVE;
	sched_start(idle);

	sched_ticker_init();

	return 0;
}

/**
 * Called with IRQs off and thread lock held.
 */
void __sched_start(struct thread *t) {
	assert(t);
	assert(!sched_ready(t));

	if (!sched_active(t)) {
		t->state |= THREAD_READY;
		runq_insert(&rq.queue, t);

		if (thread_priority_get(thread_self()) < thread_priority_get(t))
			sched_post_switch();
	}
}

void sched_start(struct thread *t) {
	ipl_t ipl;

	assert(t);

	ipl = spin_lock_ipl(&t->lock);
	__sched_start(t);
	spin_unlock_ipl(&t->lock, ipl);
}

void sched_finish(struct thread *t) {
	assert(t);
	assert(!in_harder_critical());

	irq_lock();
	{
		assert(sched_ready(t));

		if (!sched_active(t)) {
			runq_remove(&rq.queue, t);

		} else {
			assert(t == thread_self(), "XXX SMP NIY");
			sched_post_switch();
		}

		t->state &= ~THREAD_READY;
	}
	irq_unlock();
}

int sched_change_priority(struct thread *t, sched_priority_t prior) {
	assert(t);
	assert((prior >= SCHED_PRIORITY_MIN) && (prior <= SCHED_PRIORITY_MAX));

	irq_lock();
	{
		int in_runq = (sched_ready(t) && !sched_active(t));

		if (in_runq)
			runq_remove(&rq.queue, t);

		thread_priority_set(t, prior);

		if (in_runq) {
			runq_insert(&rq.queue, t);

			if (thread_priority_get(thread_self()) < prior)
				sched_post_switch();
		}
	}
	irq_unlock();

	return 0;
}

void sched_post_switch(void) {
	irq_lock();
	{
		switch_posted = 1;
		critical_request_dispatch(&sched_critical);
	}
	irq_unlock();
}

void sched_switch(void) {
	ipl_disable();
	sched_preempt();
	ipl_enable();
}

/**
 * Called by critical dispatching code with IRQs disabled.
 */
static void sched_preempt(void) {
	struct thread *prev, *next;

	assert(!in_sched_locked());
	assert(!in_harder_critical());

	sched_lock();
	while (switch_posted) {
		switch_posted = 0;

		prev = thread_get_current();

		if (sched_ready(prev))
			runq_insert(&rq.queue, prev);

		next = runq_extract(&rq.queue);

		assert(sched_ready(next));

		if (prev == next)
			continue;

		prev->state &= ~THREAD_ACTIVE;

		sched_ticker_switch(prev, next);
		sched_timing_switch(prev, next);

		next->state |= THREAD_ACTIVE;

		trace_point("context switch");

		thread_set_current(next);
		context_switch(&prev->context, &next->context);
	}
	sched_unlock_noswitch();

	thread_signal_handle();
}


