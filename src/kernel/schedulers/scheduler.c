/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 * @author Skorodumov Kirill
 */

#include <errno.h>
#include <lib/list.h>
#include <kernel/scheduler.h>
#include <kernel/scheduler_base.h>
#include <kernel/timer.h>
#include <hal/context.h>
#include <hal/ipl.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(scheduler_init);

/** Timer, which calls scheduler_tick. */
#define THREADS_TIMER_ID 17

/** Interval, what scheduler_tick is called in. */
#define THREADS_TIMER_INTERVAL 100

/**
 * If it doesn't equal to zero,
 * we are located in critical section
 * and can't switch between threads.
 */
static int preemption_count = 1;

/**
 * Initializes scheduler.
 */
static int scheduler_init(void) {
	_scheduler_init();
	current_thread = idle_thread;
	current_thread->reschedule = false;
	return 0;
}

/**
 * Is regularly called to show that current thread to be changed.
 * @param id nothing significant
 */
static void scheduler_tick(uint32_t id) {
	LOG_DEBUG("\nTick\n");
	current_thread->reschedule = true;
}

void scheduler_start(void) {
	LOG_DEBUG("\nStart scheduler\n");
	set_timer(THREADS_TIMER_ID, THREADS_TIMER_INTERVAL, scheduler_tick);
	idle_thread->reschedule = true;
	_scheduler_start();
	scheduler_unlock();
	LOG_DEBUG("\nPreemtion count = %d", preemption_count);
	LOG_DEBUG("\nCurrent thread reschedule = %d\n", current_thread->reschedule);
}

void scheduler_lock(void) {
	preemption_count++;
}

void scheduler_unlock(void) {
	preemption_count--;
	if (preemption_count == 0 && current_thread->reschedule) {
		scheduler_dispatch();
	}
}

static void preemption_inc(void) {
	preemption_count++;
}

void scheduler_dispatch(void) {
	ipl_t ipl;
	struct thread *prev_thread;

	if (preemption_count == 0 && current_thread->reschedule) {
		preemption_inc();
		prev_thread = current_thread;
		current_thread = _scheduler_next(current_thread);
		current_thread->reschedule = false;

		LOG_DEBUG("\nSwitching from %d to %d\n",
			prev_thread->id, current_thread->id);
		ipl = ipl_save();
		preemption_count--;
		context_switch(&prev_thread->context, &current_thread->context);
		ipl_restore(ipl);
	}
}

void scheduler_add(struct thread *added_thread) {
	_scheduler_add(added_thread);
}

void scheduler_stop(void) {
	scheduler_lock();
	LOG_DEBUG("\nStop scheduler\n");
	close_timer (THREADS_TIMER_ID);
	_scheduler_stop();
}

int scheduler_remove(struct thread *removed_thread) {
	if (removed_thread == NULL || removed_thread == idle_thread) {
		return -EINVAL;
	}
	scheduler_lock();
	_scheduler_remove(removed_thread);
	removed_thread->reschedule = true;
	scheduler_unlock();
	return 0;
}

int scheduler_sleep(struct event *event) {
	scheduler_lock();
	current_thread->state = THREAD_STATE_WAIT;
	list_add(&current_thread->wait_list, &event->threads_list);
	scheduler_remove(current_thread);
	LOG_DEBUG("\nLocking %d\n", current_thread->id);
	scheduler_unlock();
	return 0;
}

int scheduler_wakeup(struct event *event) {
	struct thread *thread;
	struct thread *tmp_thread;
	scheduler_lock();
	list_for_each_entry_safe(thread, tmp_thread,
			&event->threads_list, wait_list) {
		list_del_init(&thread->wait_list);
		thread->state = THREAD_STATE_RUN;
		scheduler_add(thread);
		LOG_DEBUG("\nUnlocking %d\n", thread->id);
	}
	scheduler_unlock();
	return 0;
}

int scheduler_wakeup_first(struct event *event) {
	struct thread *thread;
	scheduler_lock();
	thread = list_entry(event->threads_list.next, struct thread, wait_list);
	list_del_init(&thread->wait_list);
	thread->state = THREAD_STATE_RUN;
	scheduler_add(thread);
	LOG_DEBUG("\nUnlocking %d\n", thread->id);
	scheduler_unlock();
	return 0;
}

void event_init(struct event *event) {
	INIT_LIST_HEAD(&event->threads_list);
}
