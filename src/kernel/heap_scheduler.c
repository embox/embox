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

#include <kernel/heap_scheduler.h>
#include <kernel/thread_heap.h>
#include <kernel/timer.h>
#include <hal/context.h>
#include <hal/ipl.h>
#include <embox/unit.h>

//EMBOX_UNIT_INIT(scheduler_init);

/** Timer, which calls scheduler_tick. */
#define THREADS_TIMER_ID 17

/** Interval, what scheduler_tick is called in. */
#define THREADS_TIMER_INTERVAL 100

/** List item, pointing at begin of the (waiting) list. */
static struct list_head *list_begin_wait;
/** List item, pointing at begin of the (sleeping) list. */
static struct list_head *list_begin_sleep;

/**
 * If it doesn't equal to zero,
 * we are located in critical section
 * and can't switch between threads.
 */
static int preemption_count = 1;

int scheduler_init(void) {
	current_thread = idle_thread;
	list_begin_wait = &idle_thread->wait_list;
	list_begin_sleep = &idle_thread->sleep_list;
	current_thread->reschedule = true;
	return 0;
}

/**
 * Is regularly called to show that current thread to be changed.
 * @param id nothing significant
 */
static void scheduler_tick(uint32_t id) {
	current_thread->reschedule = true;
}

void scheduler_start(void) {
	set_timer(THREADS_TIMER_ID, THREADS_TIMER_INTERVAL, scheduler_tick);
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

/**
 * Move current_thread pointer to the next thread.
 * @param prev_thread thread, which have worked just now.
 */
static inline void thread_move_next(struct thread *prev_thread) {
	if (current_thread->state == THREAD_STATE_RUN) {
		heap_insert(current_thread);
	}
	//heap_print();
	current_thread = heap_extract();
	//heap_print();
	current_thread->reschedule = false;
}

void scheduler_dispatch(void) {
	/* Thread, which have worked just now. */
	struct thread *prev_thread;
	ipl_t ipl;

	if (preemption_count == 0 && current_thread->reschedule) {
		preemption_inc();
		prev_thread = current_thread;
		thread_move_next(prev_thread);
		//TRACE("Switching from %d to %d\n", prev_thread->id, current_thread->id);

		ipl = ipl_save();
		preemption_count--;
		context_switch(&prev_thread->context, &current_thread->context);
		ipl_restore(ipl);
	}
}

void scheduler_add(struct thread *added_thread) {
	heap_insert(added_thread);
}

int scheduler_remove(struct thread *removed_thread) {
	if (removed_thread == NULL || removed_thread == idle_thread) {
		return -EINVAL;
	}
	scheduler_lock();
	if (heap_contains(removed_thread)) {
		heap_delete(removed_thread);
	}
	scheduler_unlock();
	return 0;
}

int scheduler_add_sleep(struct thread *added_thread) {
	if (added_thread == NULL || added_thread == idle_thread) {
		return -EINVAL;
	}
	added_thread->state = THREAD_STATE_SLEEP;
	list_add_tail(&added_thread->sleep_list, list_begin_sleep);
	list_del(&added_thread->sched_list);
	return 0;
}

void scheduler_wake_up(void) {
	while (list_begin_wait->next != NULL) {
		//sleep to wait
		list_add_tail(list_begin_wait->next, list_begin_sleep);
		list_del(list_begin_wait->next);
	}
}
