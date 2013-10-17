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
#include <kernel/thread/state.h>

#include <profiler/tracing/trace.h>


static void post_switch_if(int condition);

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

	sched_wait_init();
	runq_init(&rq);

	runq_start(&rq, idle);

	sched_ticker_init();

	return 0;
}

void sched_start(struct thread *t) {
	assert(t);
	assert(!in_harder_critical());
	assert(!thread_state_active(t->state));

	sched_lock();
	{
		post_switch_if(runq_start(&rq, t));
	}
	sched_unlock();
}

void sched_wake(struct thread *t) {
	assert(in_sched_locked());

	post_switch_if(runq_wake_thread(&rq, t));
}

void sched_sleep(struct thread *t) {
	assert(in_sched_locked() && !in_harder_critical());
	assert(thread_state_running(t->state));

	runq_wait(&rq);

	assert(thread_state_sleeping(t->state));

	sched_post_switch();
}

void sched_finish(struct thread *t) {
	assert(!in_harder_critical());

	sched_lock();
	{
		assert(!thread_state_exited(t->state));

		if (thread_state_running(t->state)) {
			post_switch_if(runq_finish(&rq, t));
		} else {
			if (thread_state_sleeping(t->state)) {
				t->wait_data.on_notified(t, t->wait_data.data);
			}
			t->state = thread_state_do_exit(t->state);
		}

		assert(thread_state_exited(t->state));
	}
	sched_unlock();
}

void sched_post_switch(void) {
	sched_lock();
	{
		post_switch_if(1);
	}
	sched_unlock();
}

int sched_change_priority(struct thread *t, sched_priority_t prior) {
	assert(t);
	assert((prior >= SCHED_PRIORITY_MIN) && (prior <= SCHED_PRIORITY_MAX));

	sched_lock();
	{
		assert(!thread_state_exited(t->state));

		thread_priority_set(t, prior);

		if (thread_state_running(t->state)) {
			post_switch_if(runq_change_priority(&rq, t, prior));
		}

		assert(thread_priority_get(t) == prior);
	}
	sched_unlock();

	return 0;
}


static void post_switch_if(int condition) {
	assert(in_sched_locked());

	if (condition) {
		switch_posted = 1;
		critical_request_dispatch(&sched_critical);
	}
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
		sched_wait_run();

		if (!switch_posted) {
			goto out;
		}
		switch_posted = 0;

		ipl_enable();

		prev = thread_get_current();

		next = runq_switch(&rq);

		if (prev == next) {
			ipl_disable();
			goto out;
		}

		assert(thread_state_running(next->state));

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

	thread_signal_handle();
}


