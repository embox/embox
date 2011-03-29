/**
 * @file
 * @brief Implementation of methods in scheduler.h
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

/** Timer, which calls scheduler_tick. */
#define THREADS_TIMER_ID 17

/** Interval, what scheduler_tick is called in. */
#define THREADS_TIMER_INTERVAL 100

int sched_init(struct thread* current, struct thread *idle) {
	sched_policy_init(current, idle);
	return 0;
}

/**
 * Is regularly called to show that current thread to be changed.
 * @param id nothing significant
 */
static void sched_tick(uint32_t id) {
	LOG_DEBUG("\nTick\n");
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

void sched_add(struct thread *added_thread) {
	struct thread *current;
	bool resched;

	sched_lock();

	resched = sched_policy_add(added_thread);
	current = sched_current();

	assert(current);

	current->reschedule |= resched;

	sched_unlock();
}

void sched_stop(void) {
	sched_lock();
	LOG_DEBUG("\nStop scheduler\n");
	close_timer(THREADS_TIMER_ID);
	sched_policy_stop();
}

int sched_remove(struct thread *removed_thread) {
	if (removed_thread == NULL || removed_thread == idle_thread) {
		return -EINVAL;
	}
	sched_lock();
	sched_current()->reschedule |= sched_policy_remove(removed_thread);
	sched_unlock();
	return 0;
}

int scher_sleep(struct event *event) {
	sched_lock();
	sched_current()->state = THREAD_STATE_WAIT;
	list_add(&sched_current()->wait_list, &event->threads_list);
	sched_current()->reschedule |= sched_policy_remove(sched_current());
	LOG_DEBUG("\nLocking %d\n", thread_current()->id);
	sched_unlock();
	return 0;
}

int sched_wakeup(struct event *event) {
	struct thread *thread;
	struct thread *tmp_thread;
	sched_lock();
	list_for_each_entry_safe(thread, tmp_thread,
			&event->threads_list, wait_list) {
		list_del_init(&thread->wait_list);
		thread->state = THREAD_STATE_RUN;
		sched_current()->reschedule |= sched_policy_add(thread);
		LOG_DEBUG("\nUnlocking %d\n", thread->id);
	}
	sched_unlock();
	return 0;
}

int sched_wakeup_first(struct event *event) {
	struct thread *thread;
	sched_lock();
	thread = list_entry(event->threads_list.next, struct thread, wait_list);
	list_del_init(&thread->wait_list);
	thread->state = THREAD_STATE_RUN;
	sched_current()->reschedule |= sched_policy_add(thread);
	LOG_DEBUG("\nUnlocking %d\n", thread->id);
	sched_unlock();
	return 0;
}

void event_init(struct event *event) {
	INIT_LIST_HEAD(&event->threads_list);
}
