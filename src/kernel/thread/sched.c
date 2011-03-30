/**
 * @file
 * @brief Implementation of methods in sched.h
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 * @author Skorodumov Kirill
 */

#include <assert.h>
#include <errno.h>

#include <lib/list.h>
#include <kernel/critical/api.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/sched_policy.h>
#include <kernel/timer.h>
#include <hal/context.h>
#include <hal/ipl.h>
#include <embox/unit.h>
#include "state.h"

/** Timer, which calls scheduler_tick. */
#define THREADS_TIMER_ID 17

/** Interval, what scheduler_tick is called in. */
#define THREADS_TIMER_INTERVAL 100

int sched_init(struct thread* current, struct thread *idle) {
	current->state = thread_state_transition(current->state,
			THREAD_STATE_ACTION_START);

	current->state = thread_state_transition(current->state,
			THREAD_STATE_ACTION_RUN);
	assert(current->state == THREAD_STATE_RUNNING);

	idle->state = thread_state_transition(idle->state,
			THREAD_STATE_ACTION_START);
	assert(idle->state == THREAD_STATE_READY);

	sched_policy_init(current, idle);
	return 0;
}

/**
 * Is regularly called to show that current thread to be changed.
 * @param id nothing significant
 */
static void sched_tick(uint32_t id) {
	sched_current()->reschedule = true;
}

void sched_start(void) {
	set_timer(THREADS_TIMER_ID, THREADS_TIMER_INTERVAL, sched_tick);
	idle_thread->reschedule = true;
	sched_policy_start();

	sched_unlock();
}

/**
 * Switches thread to another thread and their contexts.
 */
static void sched_switch(void) {
	ipl_t ipl;
	struct thread *current, *next;

	current = sched_current();
	next = sched_policy_switch(current);

	if (next == current) {
		return;
	}

	if (current->state == THREAD_STATE_RUNNING) {
		current->state = thread_state_transition(current->state,
				THREAD_STATE_ACTION_PREEMPT);
	}

	assert(current->state);

	next->state = thread_state_transition(next->state, THREAD_STATE_ACTION_RUN);
	assert(next->state);

#ifdef CONFIG_PP_ENABLE
	if (next->pp != current->pp) {
		pp_store(current->pp);
		pp_restore(next->pp);
	}
#endif

	ipl = ipl_save();
	context_switch(&current->context, &next->context);
	ipl_restore(ipl);

}

void sched_dispatch(void) {
	struct thread *current;

	assert(critical_allows(CRITICAL_PREEMPT));

	sched_lock();

	current = sched_current();

	while (current->reschedule) {
		current->reschedule = false;
		sched_switch();
	}

	sched_unlock();
}

void sched_add(struct thread *t) {
	struct thread *current;

	sched_lock();

	t->state = thread_state_transition(t->state, THREAD_STATE_ACTION_START);
	assert(t->state);

	current = sched_current();
	assert(current);

	current->reschedule |= sched_policy_add(t);

	sched_unlock();
}

void sched_stop(void) {
	sched_lock();

	close_timer(THREADS_TIMER_ID);
	sched_policy_stop();
}

int sched_remove(struct thread *t) {
	if (t == NULL || t == idle_thread) {
		return -EINVAL;
	}

	sched_lock();

	sched_current()->reschedule |= sched_policy_remove(t);
	t->state = thread_state_transition(t->state, THREAD_STATE_ACTION_STOP);

	sched_unlock();

	return 0;
}

int scher_sleep(struct event *event) {
	struct thread *current;

	sched_lock();

	current = sched_current();
	current->state = thread_state_transition(current->state,
			THREAD_STATE_ACTION_SLEEP);
	list_add(&current->wait_list, &event->threads_list);
	current->reschedule |= sched_policy_remove(current);

	sched_unlock();

	return 0;
}

int sched_wakeup(struct event *event) {
	struct thread *t, *tmp;
	struct thread *current;

	sched_lock();

	current = sched_current();

	list_for_each_entry_safe(t, tmp, &event->threads_list, wait_list) {
		list_del_init(&t->wait_list);
		t->state = thread_state_transition(t->state, THREAD_STATE_ACTION_WAKE);
		current->reschedule |= sched_policy_add(t);
	}

	sched_unlock();

	return 0;
}

int sched_wakeup_first(struct event *event) {
	struct thread *t;

	sched_lock();

	t = list_entry(event->threads_list.next, struct thread, wait_list);
	list_del_init(&t->wait_list);
	t->state = thread_state_transition(t->state, THREAD_STATE_ACTION_WAKE);
	sched_current()->reschedule |= sched_policy_add(t);

	sched_unlock();

	return 0;
}

void event_init(struct event *event) {
	INIT_LIST_HEAD(&event->threads_list);
}
