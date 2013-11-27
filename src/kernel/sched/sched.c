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

void sched_post_switch(void) {
	critical_request_dispatch(&sched_critical);
}

static inline int sched_in_interrupt(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

int sched_init(struct thread *idle, struct thread *current) {
	assert(idle && current);

	runq_init(&rq.queue);

	current->state = THREAD_READY | THREAD_ACTIVE;
	sched_start(idle);

	sched_ticker_init();

	return 0;
}

/** Called with IRQs off and thread lock held. */
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
	assert(t);
	SPIN_IPL_PROTECTED_DO(&t->lock, __sched_start(t));
}

/** Called with IRQs off and thread lock held. */
int __sched_wakeup(struct thread *t) {
	assert(t);

	if (t->state & THREAD_READY)
		return 0;

	__sched_start(t);

	return 1;
}

int sched_wakeup(struct thread *t) {
	assert(t);
	return SPIN_IPL_PROTECTED_DO(&t->lock, __sched_wakeup(t));
}

void sched_finish(struct thread *t) {
	assert(t);
	assert(!sched_in_interrupt());

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

static void sched_switch(struct thread *prev, struct thread *next) {
	unsigned int local_critical = critical_count();
	critical_count() = __CRITICAL_COUNT(CRITICAL_SCHED_LOCK);

	assert(prev != next);

	prev->state &= ~THREAD_ACTIVE;

	sched_ticker_switch(prev, next);
	sched_timing_switch(prev, next);

	next->state |= THREAD_ACTIVE;

	trace_point(__func__);

	thread_set_current(next);
	context_switch(&prev->context, &next->context);

	critical_count() = local_critical;
}

static void __schedule(int preempt) {
	struct thread *prev, *next;

	assert(!sched_in_interrupt());

	prev = thread_self();

	if (sched_ready(prev)) {
		if (!preempt)
			return;
		runq_insert(&rq.queue, prev);
	}

	next = runq_extract(&rq.queue);
	assert(sched_ready(next));

	if (prev != next)
		sched_switch(prev, next);

	thread_signal_handle();
}

/** Called by critical dispatching code with IRQs disabled. */
static void sched_preempt(void) {
	__schedule(1);
}

void schedule(void) {
	ipl_disable();
	__schedule(0);
	ipl_enable();
}


