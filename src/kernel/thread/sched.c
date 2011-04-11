/**
 * @file
 * @brief TODO --Alina
 *
 * @date 22.04.2010
 * @author Dmitry Avdyukhin
 * @author Kirill Skorodumov
 * @author Alina Kramar
 */

#include <assert.h>
#include <errno.h>

#include <embox/unit.h>
#include <lib/list.h>
#include <kernel/critical/api.h>
#include <kernel/thread/api.h>
#include <kernel/thread/event.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/sched_policy.h>
#include <kernel/timer.h>
#include <hal/context.h>
#include <hal/ipl.h>

#include "types.h"
#include "state.h"

/** Timer, which calls scheduler_tick. */
#define SCHED_TICK_TIMER_ID 17

/** Interval, what scheduler_tick is called in. */
#define SCHED_TICK_INTERVAL 100

EMBOX_UNIT(unit_init, unit_fini);

int sched_init(struct thread* current, struct thread *idle) {
	int error;

	if ((error = sched_policy_init(current, idle))) {
		return error;
	}

	current->state = thread_state_transition(current->state,
			THREAD_STATE_ACTION_START);
	assert(current->state == THREAD_STATE_RUNNING);

	idle->state = thread_state_transition(idle->state,
			THREAD_STATE_ACTION_START);
	assert(idle->state == THREAD_STATE_RUNNING);

	sched_unlock();

	return 0;
}

/**
 * Is regularly called to show that current thread to be changed.
 * @param id nothing significant
 */
static void sched_tick(uint32_t id) {
	sched_current()->resched = true;
}

/**
 * Switches thread to another thread and their contexts.
 */
static void sched_switch(void) {
	struct thread *current, *next;
	ipl_t ipl;

	current = sched_current();
	next = sched_policy_switch(current);

	if (next == current) {
		return;
	}

	ipl = ipl_save();
	context_switch(&current->context, &next->context);
	ipl_restore(ipl);

}

void __sched_dispatch(void) {
	struct thread *current;

	assert(critical_allows(CRITICAL_PREEMPT));

	sched_lock();

	current = sched_current();

	while (current->resched) {
		current->resched = false;
		sched_switch();
	}

	sched_unlock();
}

void sched_start(struct thread *t) {
	struct thread *current;

	sched_lock();

	current = sched_current();
	assert(current);

	current->resched |= sched_policy_start(t);

	t->state = thread_state_transition(t->state, THREAD_STATE_ACTION_START);
	assert(t->state);

	sched_unlock();
}

void sched_stop(struct thread *t) {
	assert(t != NULL);

	sched_lock();

	sched_current()->resched |= sched_policy_stop(t);

	t->state = thread_state_transition(t->state, THREAD_STATE_ACTION_STOP);
	assert(t->state);

	sched_unlock();
}

int sched_sleep_locked(struct event *e) {
	struct thread *current;

	assert(e);
	assert(critical_inside(CRITICAL_PREEMPT));

	current = sched_current();

	current->resched |= sched_policy_stop(current);

	current->state = thread_state_transition(current->state,
			THREAD_STATE_ACTION_SLEEP);
	assert(current->state);

	list_add(&current->sched_list, &e->sleep_queue);

	sched_unlock();
	assert(critical_allows(CRITICAL_PREEMPT));
	sched_lock();

	return 0;
}

int sched_sleep(struct event *e) {
	int ret;

	assert(e);
	assert(critical_allows(CRITICAL_PREEMPT));

	sched_lock();

	ret = sched_sleep_locked(e);

	sched_unlock_noswitch();

	return ret;
}

int sched_wake(struct event *e) {
	struct thread *t, *tmp;
	struct thread *current;

	sched_lock();

	current = sched_current();

	list_for_each_entry_safe(t, tmp, &e->sleep_queue, sched_list) {
		list_del_init(&t->sched_list);

		current->resched |= sched_policy_start(t);

		t->state = thread_state_transition(t->state, THREAD_STATE_ACTION_WAKE);
		assert(t->state);
	}

	sched_unlock();

	return 0;
}

int sched_wake_one(struct event *e) {
	struct thread *t;

	sched_lock();

	t = list_entry(e->sleep_queue.next, struct thread, sched_list);
	list_del_init(&t->sched_list);
	sched_current()->resched |= sched_policy_start(t);
	t->state = thread_state_transition(t->state, THREAD_STATE_ACTION_WAKE);

	sched_unlock();

	return 0;
}

void sched_yield(void) {
	sched_lock();

	sched_current()->resched = true;

	sched_unlock();
}

void sched_suspend(struct thread *t){

	sched_policy_stop(t);

	t->state = thread_state_transition(t->state, THREAD_STATE_ACTION_SUSPEND);

	thread_self()->resched = (t == thread_self());
}

void sched_resume(struct thread *t) {

	sched_policy_start(t);

	t->state = thread_state_transition(t->state, THREAD_STATE_ACTION_RESUME);

	thread_self()->resched = true;

}


static int unit_init(void) {
	if (set_timer(SCHED_TICK_TIMER_ID, SCHED_TICK_INTERVAL, sched_tick)
			!= SCHED_TICK_TIMER_ID) {
		return -EBUSY;
	}

	return 0;
}

static int unit_fini(void) {
	close_timer(SCHED_TICK_TIMER_ID);

	return 0;
}
