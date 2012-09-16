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
#include <lib/list.h>

#include <kernel/critical.h>
#include <kernel/irq_lock.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>
#include <kernel/time/timer.h>
#include <kernel/task.h>
#include <hal/context.h>
#include <hal/ipl.h>

#include <profiler/tracing/trace.h>

#include <time.h>

#include "types.h"

#include <embox/unit.h>

/** Interval, what scheduler_tick is called in. */
#define SCHED_TICK_INTERVAL 100

EMBOX_UNIT(unit_init, unit_fini);

extern void startq_flush(void);
extern void startq_enqueue_wake(struct sleepq *sq, int wake_all);
extern void startq_enqueue_wake_force(struct thread *t);

int do_thread_wake_force(struct thread *thread);
void do_sleepq_wake(struct sleepq *sq, int wake_all);

static void do_sleep_locked(struct sleepq *sq);

static void __sched_wake(struct sleepq *sq, int wake_all);

static void post_switch_if(int condition);

static void sched_switch(void);

CRITICAL_DISPATCHER_DEF(sched_critical, sched_switch, CRITICAL_SCHED_LOCK);

static struct runq rq;

static sys_timer_t *tick_timer;

static clock_t prev_clock;

static inline int in_harder_critical(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

static inline int in_sched_locked(void) {
	return !critical_allows(CRITICAL_SCHED_LOCK);
}

int sched_init(struct thread* current, struct thread *idle) {
	prev_clock = clock();

	runq_init(&rq, current, idle);

	assert(thread_state_started(current->state));
	assert(thread_state_started(idle->state));

	return 0;
}

struct thread *sched_current(void) {
	return runq_current(&rq);
}

void sched_start(struct thread *t) {
	assert(!in_harder_critical());

	sched_lock();
	{
		assert(!thread_state_started(t->state));
		post_switch_if(runq_start(&rq, t));
		assert(thread_state_started(t->state));
	}
	sched_unlock();
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
				sleepq_finish(t->sleepq, t);
			} else {
				t->state = thread_state_do_exit(t->state);
			}
		}

		assert(thread_state_exited(t->state));
	}
	sched_unlock();
}

void sched_wake_all(struct sleepq *sq) {
	__sched_wake(sq, 1);
}

void sched_wake_one(struct sleepq *sq) {
	__sched_wake(sq, 0);
}

void __sched_wake(struct sleepq *sq, int wake_all) {
	if (in_harder_critical()) {
		startq_enqueue_wake(sq, wake_all);
		critical_request_dispatch(&sched_critical);
	} else {
		do_sleepq_wake(sq, wake_all);
	}
}

void do_sleepq_wake(struct sleepq *sq, int wake_all) {
	assert(!in_harder_critical());

	sched_lock();
	{
		post_switch_if(sleepq_wake(&rq, sq, wake_all));
	}
	sched_unlock();
}

static int thread_wake_force(struct thread *thread, int sleep_result) {
	thread->sleep_res = sleep_result;

	if (in_harder_critical()) {
		startq_enqueue_wake_force(thread);
		critical_request_dispatch(&sched_critical);
	} else {
		return do_thread_wake_force(thread);
	}

	return 0;
}

int do_thread_wake_force(struct thread *thread) {
	assert(!in_harder_critical());
	assert(thread_state_sleeping(thread->state));

	return sleepq_wake_thread(&rq, thread->sleepq, thread);
}

static void do_sleep_locked(struct sleepq *sq) {
	struct thread *current = runq_current(&rq);
	assert(in_sched_locked() && !in_harder_critical());
	assert(thread_state_running(current->state));

	runq_sleep(&rq, sq);

	assert(current->sleepq == sq);
	assert(thread_state_sleeping(current->state));

	post_switch_if(1);
}

static void timeout_handler(struct sys_timer *timer, void *sleep_data) {
	struct thread *thread = (struct thread *) sleep_data;
	post_switch_if(thread_wake_force(thread, SCHED_SLEEP_TIMEOUT));
}

int sched_sleep_locked(struct sleepq *sq, unsigned long timeout) {
	int ret;
	struct sys_timer tmr;
	struct thread *current = sched_current();

	assert(in_sched_locked() && !in_harder_critical());
	assert(thread_state_running(current->state));

	current->sleep_res = 0; /* clean out sleep_res */

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		ret = timer_init(&tmr, TIMER_ONESHOT, (uint32_t)timeout, timeout_handler, current);
		if (ret != ENOERR) {
			return ret;
		}
	}

	do_sleep_locked(sq);

	sched_unlock();

	/* At this point we have been awakened and are ready to go. */
	assert(!in_sched_locked());
	assert(thread_state_running(current->state));

	sched_lock();

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		timer_close(&tmr);
	}

	return current->sleep_res;
}

int sched_sleep(struct sleepq *sq, unsigned long timeout) {
	int sleep_res;
	assert(!in_sched_locked());

	sched_lock();
	{
		sleep_res = sched_sleep_locked(sq, timeout);
	}
	sched_unlock();

	return sleep_res;
}

#if 0
int sched_setrun(struct thread *t) {
	sched_lock();
	{
		if (thread_state_sleeping(t->state)) {
			thread_wake_force(t, SCHED_SLEEP_INTERRUPT);
		}
		/*
		if (thread_state_suspended(t->state)) {
			do_thread_resume(t);
		}
		*/
	}
	sched_unlock();
	return 0;
}
#endif

void sched_yield(void) {
	sched_lock();
	{
		post_switch_if(1);
	}
	sched_unlock();
}

int sched_change_scheduling_priority(struct thread *t,
		__thread_priority_t new_priority) {
	sched_lock();
	{
		assert(!thread_state_exited(t->state));

		if (thread_state_running(t->state)) {
			post_switch_if(runq_change_priority(t->runq, t, new_priority));
		} else if (thread_state_sleeping(t->state)) {
			sleepq_change_priority(t->sleepq, t, new_priority);
		} else {
			t->priority = new_priority;
		}

		assert(t->priority == new_priority);
	}
	sched_unlock();

	return 0;
}

void sched_set_priority(struct thread *t, __thread_priority_t new) {
	sched_lock();
	{
		if (!thread_state_exited(t->state)) {
			sched_change_scheduling_priority(t, new);
		}
		t->initial_priority = new;
	}
	sched_unlock();
}

static int switch_posted;

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
		startq_flush();

		if (!switch_posted) {
			goto out;
		}
		switch_posted = 0;

		ipl_enable();

		prev = runq_current(&rq);

		new_clock = clock();
		prev->running_time += new_clock - prev_clock;
		prev_clock = new_clock;

		if (!runq_switch(&rq)) {
			ipl_disable();
			goto out;
		}

		next = runq_current(&rq);

		assert(thread_state_running(next->state));
		assert(next != prev);

		trace_point("context switch");

		ipl_disable();

		task_notify_switch(prev, next);
		context_switch(&prev->context, &next->context);
	}

out:
	sched_unlock_noswitch();
}

static void sched_tick(sys_timer_t *timer, void *param) {
	post_switch_if(1);
}

static int unit_init(void) {
	if (timer_set(&tick_timer, TIMER_PERIODIC, SCHED_TICK_INTERVAL, sched_tick, NULL)) {
		return -EBUSY;
	}

	return 0;
}

static int unit_fini(void) {
	timer_close(tick_timer);

	return 0;
}
