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

#include <kernel/runnable/runnable.h>
#include <kernel/thread/current.h>
#include <kernel/thread/signal.h>
#include <kernel/thread/state.h>

#include <profiler/tracing/trace.h>

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
		runq_queue_insert(&rq.queue, &(t->runnable));

		if (thread_priority_get(t) > thread_priority_get(current)) {
			sched_post_switch();
		}
	}
	sched_unlock();
}

/* TODO move to waitq, haven't been refactored yet */
void sched_sleep(void) {
	struct thread *current = thread_get_current();
	assert(!in_harder_critical());

	assert(in_sched_locked() && !in_harder_critical());
	assert(current->state & (__THREAD_STATE_READY | __THREAD_STATE_RUNNING));

	current->state = __THREAD_STATE_WAITING;
	/* we don't remove current because it is not in runq, we just mark it as
	 * waiting and after sched switch all will be correct
	 */

	assert(__THREAD_STATE_WAITING & current->state);

	sched_post_switch();
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
				runq_queue_remove(&rq.queue, &(t->runnable));
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
			runq_queue_remove(&rq.queue, &(t->runnable));
			runq_queue_insert(&rq.queue, &(t->runnable));

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
	struct runnable *prev, *next;

	assert(!in_sched_locked());

	sched_lock();
	{
		if (!switch_posted) {
			goto out;
		}
		switch_posted = 0;

		ipl_enable();

		prev = runnable_get_current();

		/*prev can be thread, we might want to put it into runq */
		if(prev->sched_thread_specific != NULL) {
			prev->sched_thread_specific(prev, &rq);
		}

		next = runq_queue_extract(&rq.queue);

		/* Start execution, context switching for threads inside*/
		next->run(prev, next, &rq);
	}
out:
	sched_unlock_noswitch();

	thread_signal_handle();
}

/* Implement from runnable.h */
struct runnable *runnable_get_current(){
	return &(thread_get_current()->runnable);
}
