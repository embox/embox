/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#include <errno.h>
#include <stdlib.h>
#include <lib/list.h>

#include <kernel/scheduler.h>
#include <kernel/scheduler_base.h>
#include <kernel/timer.h>
#include <hal/context.h>
#include <hal/ipl.h>
#include <embox/unit.h>

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

int scheduler_init(void) {
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
	TRACE("\nTick\n");
	current_thread->reschedule = true;
}

void scheduler_start(void) {
	TRACE("\nStart scheduler\n");
	set_timer(THREADS_TIMER_ID, THREADS_TIMER_INTERVAL, scheduler_tick);
	_scheduler_start();
	scheduler_unlock();
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

void scheduler_switch(struct thread old_thread, struct thread new_thread) {
	ipl_t ipl;

	ipl = ipl_save();
	preemption_count--;
	context_switch(&old_thread.context, &new_thread.context);
	ipl_restore(ipl);
}

void scheduler_dispatch(void) {
	/* Thread, which have worked just now. */
	struct thread *prev_thread;
	struct thread *last_prev_thread;

	if (preemption_count == 0 && current_thread->reschedule) {
		preemption_inc();
		prev_thread = current_thread;
		/* Search first running thread. */
		last_prev_thread = current_thread;
		thread_move_next(last_prev_thread);
		while (current_thread->state != THREAD_STATE_RUN) {
			last_prev_thread = current_thread;
			thread_move_next(last_prev_thread);
		}
		current_thread->reschedule = false;
		TRACE("Switching from %d to %d\n", prev_thread->id, current_thread->id);

		scheduler_switch(*prev_thread, *current_thread);
	}
}

void scheduler_add(struct thread *added_thread) {
	_scheduler_add(added_thread);
}

int scheduler_remove(struct thread *removed_thread) {
	if (removed_thread == NULL || removed_thread == idle_thread) {
		return -EINVAL;
	}
	scheduler_lock();
	_scheduler_remove(removed_thread);
	scheduler_unlock();
	return 0;
}

int scheduler_sleep(struct thread *thread, struct event *event) {
	struct thread *old_thread = thread;

	preemption_inc();

	thread_move_next(thread);
	list_del_init(&old_thread->sched_list);
	old_thread->state = THREAD_STATE_WAIT;
	list_add(&old_thread->sched_list, &event->threads_list);

	TRACE("Locking %d\n", old_thread->id);

	scheduler_switch(*old_thread, *thread);

	return 0;
}

int scheduler_wakeup(struct event *event) {
	struct thread *thread;
	struct list_head *event_head = event->threads_list.next;
	scheduler_lock();
	list_del(&event->threads_list);
	list_for_each_entry(thread, event_head, sched_list) {
		list_del_init(&thread->sched_list);
		scheduler_add(thread);
		thread->state = THREAD_STATE_RUN;
		scheduler_add(thread);

		TRACE("Unlocking %d\n", thread->id);

	}

	scheduler_unlock();

	return 0;
}
