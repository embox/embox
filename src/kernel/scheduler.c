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

/** List item, pointing at begin of the list
 * 	of waiting threads
 */
static struct list_head list_head_wait;

int scheduler_init(void) {
	_scheduler_init();
	INIT_LIST_HEAD(&list_head_wait);
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

void scheduler_dispatch(void) {
	/* Thread, which have worked just now. */
	struct thread *prev_thread;
	struct thread *last_prev_thread;
	ipl_t ipl;

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

		ipl = ipl_save();
		preemption_count--;
		context_switch(&prev_thread->context, &current_thread->context);
		ipl_restore(ipl);
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

int thread_lock(struct thread *thread, struct event *event) {
	struct thread *old_thread = thread;
	ipl_t ipl;

	preemption_inc();

	list_add(&thread->wait_list, &list_head_wait);
	thread_move_next(thread);
	list_del(&old_thread->sched_list);
	old_thread->state = THREAD_STATE_WAIT;
	list_add(&old_thread->sched_list,event->threads);

	TRACE("Blocking %d\n", old_thread->id);

	ipl = ipl_save();
	preemption_count--;
	context_switch(&old_thread->context, &thread->context);
	ipl_restore(ipl);

	return 0;
}

int thread_unlock(struct event *event) {

	return 0;
}
