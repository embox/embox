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
#include <kernel/thread/event.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/state.h>
#include <kernel/timer.h>
#include <hal/context.h>
#include <hal/ipl.h>
#include <util/slist.h>

#include "types.h"

#include <embox/unit.h>

/** Interval, what scheduler_tick is called in. */
#define SCHED_TICK_INTERVAL 100

EMBOX_UNIT(unit_init, unit_fini);

static void startq_flush(void);
static void startq_enqueue_wake(struct event *e, int wake_all);
static void startq_enqueue_resume(struct thread *t);

static void do_thread_resume(struct thread *t);
static void do_event_wake(struct event *e, int wake_all);

static void post_switch_if(int condition);

static void sched_switch(void);

CRITICAL_DISPATCHER_DEF(sched_critical, sched_switch, CRITICAL_SCHED_LOCK);

static struct runq rq;

static sys_timer_t *tick_timer;

static inline int in_harder_critical(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

static inline int __attribute__((unused)) in_sched_locked(void) {
	return !critical_allows(CRITICAL_SCHED_LOCK);
}

int sched_init(struct thread* current, struct thread *idle) {
	runq_init(&rq, current, idle);
	return 0;
}

struct thread *sched_current(void) {
	return runq_current(&rq);
}

void sched_resume(struct thread *t) {
	if (in_harder_critical()) {
		startq_enqueue_resume(t);

	} else {
		do_thread_resume(t);

	}
}

static void do_thread_resume(struct thread *t) {
	assert(!in_harder_critical());

	sched_lock();
	{
		thread_state_t state = t->state = thread_state_do_resume(t->state);

		if (thread_state_running(state)) {
			t->runq = &rq; // XXX
			post_switch_if(runq_start(t->runq, t));
		}
	}
	sched_unlock();
}

void sched_suspend(struct thread *t) {
	assert(!in_harder_critical());

	sched_lock();
	{
		thread_state_t state = t->state;

		if (thread_state_running(state)) {
			post_switch_if(runq_stop(t->runq, t));
			t->runq = NULL; // XXX
		}

		t->state = thread_state_do_suspend(state);
	}
	sched_unlock();
}

void __sched_wake(struct event *e, int wake_all) {
	if (in_harder_critical()) {
		startq_enqueue_wake(e, wake_all);

	} else {
		do_event_wake(e, wake_all);

	}
}

static void do_event_wake(struct event *e, int wake_all) {
	assert(!in_harder_critical());

	sched_lock();
	{
		post_switch_if(runq_wake(&rq, &e->sleepq, wake_all));
	}
	sched_unlock();
}

int sched_sleep_locked(struct event *e) {
	struct thread *current = runq_current(&rq);

	post_switch_if(runq_sleep(&rq, &e->sleepq));

	sched_unlock();

	/* At this point we have been awakened and are ready to go. */
	assert(!in_sched_locked());
	assert(thread_state_running(current->state));

	sched_lock();

	return 0;
}

int sched_sleep(struct event *e) {
	assert(!in_sched_locked());

	sched_lock();
	{
		post_switch_if(runq_sleep(&rq, &e->sleepq));
	}
	sched_unlock();

	return 0;
}

void sched_yield(void) {
	post_switch_if(1);
}

int sched_change_scheduling_priority(struct thread *t,
		__thread_priority_t new_priority) {
	sched_lock();
	{
		if (thread_state_exited(t->state)) {
			assert(0);
			// XXX
			sched_unlock();
			return -ESRCH;
		}

		if (thread_state_running(t->state)) {
			post_switch_if(runq_change_priority(t->runq, t, new_priority));

		} else if (thread_state_sleeping(t->state)) {
			sleepq_change_priority(t->sleepq, t, new_priority);

		}

		assert(t->priority == new_priority);
	}
	sched_unlock();

	return 0;
}

void sched_set_priority(struct thread *t, __thread_priority_t new) {
	sched_lock();

	sched_change_scheduling_priority(t, new);
	t->initial_priority = new;

	sched_unlock();
}

static int switch_posted;

static void post_switch_if(int condition) {
	assert(in_sched_locked());
	assert(!in_harder_critical());

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
		if (!runq_switch(&rq)) {
			ipl_disable();
			goto out;
		}

		next = runq_current(&rq);

		assert(thread_state_running(next->state));
		assert(next != prev);

		ipl_disable();

		context_switch(&prev->context, &next->context);
	}
	out: sched_unlock();
}

struct startq {
	struct slist event_wake;
	struct slist thread_resume;
};

static struct startq startq;

/**
 * Called outside any interrupt handler as part of critical dispatcher
 * with IRQs disabled and the scheduler locked.
 */
static void startq_flush(void) {
	struct thread *t;
	struct event *e;

	assert(in_sched_locked());
	assert(!in_harder_critical());

	while ((e = slist_remove_first(&startq.event_wake,
			struct event, startq_link))) {
		int wake_all = e->startq_wake_all;

		ipl_enable();
		do_event_wake(e, wake_all);
		ipl_disable();
	}

	while ((t = slist_remove_first(&startq.thread_resume,
			struct thread, startq_link))) {
		ipl_enable();
		do_thread_resume(t);
		ipl_disable();
	}
}

static void startq_enqueue_wake(struct event *e, int wake_all) {
	ipl_t ipl = ipl_save();
	{
		if (slist_alone(e, startq_link)) {
			slist_add_first(e, &startq.event_wake, startq_link);
			e->startq_wake_all = wake_all;
		} else {
			e->startq_wake_all |= wake_all;
		}
	}
	ipl_restore(ipl);

	critical_request_dispatch(&sched_critical);
}

static void startq_enqueue_resume(struct thread *t) {
	ipl_t ipl = ipl_save();
	{
		if (slist_alone(t, startq_link)) {
			slist_add_first(t, &startq.thread_resume, startq_link);
		}
	}
	ipl_restore(ipl);

	critical_request_dispatch(&sched_critical);
}

static void sched_tick(sys_timer_t *timer, void *param) {
	post_switch_if(1);
}

static int unit_init(void) {
	if (timer_set(&tick_timer, SCHED_TICK_INTERVAL, sched_tick, NULL)) {
		return -EBUSY;
	}

	return 0;
}

static int unit_fini(void) {
	timer_close(tick_timer);

	return 0;
}

