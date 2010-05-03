/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#include <hal/context.h>
#include <kernel/scheduler.h>
#include <errno.h>
#include <kernel/timer.h>
#include <stdlib.h>
#include <assert.h>
#include <lib/list.h>

#define THREAD_STACK_SIZE 0x1000
#define THREADS_TIMER_ID 17
#define THREADS_TIMER_TICKS 5
#define SCHEDULER_INTERVAL 50

/**
 * If it doesn't equal to zero, it means
 * that we are located in critical section
 * and cant's switch between threads.
 */
static int preemption_count = 1;
/**
 * Thread, which makes nothing.
 * Is used to be working when there is no another process.
 */
static struct thread idle_thread;
/**
 * Thread, which have just worked.
 */
static struct thread *prev_thread;
/**
 * Thread, which works now.
 */
static struct thread *current_thread;
/**
 * Position of currently working thread in the list.
 * It is needed when we want to delete thread.
 */
static struct thread *current_pos;
/**
 * Stack for idle_thread.
 */
static char idle_thread_stack[THREAD_STACK_SIZE];
/**
 * List item, pointing at begin of the list.
 */
static struct thread *list_begin;
/**
 * Redundant thread, which will never work.
 */
static struct thread redundant_thread;

/**
 * Function, which makes nothing.
 */
static void idle_run(void) {
	while (true) {
		TRACE("?");
	}
}

void scheduler_init(void) {
	thread_create(&idle_thread, idle_run, idle_thread_stack
			+ sizeof(idle_thread_stack));
	INIT_LIST_HEAD(&idle_thread.list);
	current_thread = &idle_thread;
	current_pos = &idle_thread;
	list_begin = &idle_thread;
	current_thread->must_be_switched = false;
}

void scheduler_start(void) {
	TRACE("\nStart scheduler\n");
	list_for_each_entry(current_thread, &list_begin->list, list) {
		TRACE("%d ", current_thread->id);
	}
	set_timer(THREADS_TIMER_ID, THREADS_TIMER_TICKS, scheduler_tick);
	preemption_count--;
	context_switch(&redundant_thread.thread_context, &idle_thread.thread_context);
}


void scheduler_lock(void) {
	preemption_count++;
}

void scheduler_unlock(void) {
	preemption_count--;
	if (preemption_count == 0 && current_thread->must_be_switched) {
		scheduler_dispatch();
	}
}

void scheduler_dispatch(void) {
	if (preemption_count == 0 && current_thread->must_be_switched) {
		preemption_count++;
		prev_thread = current_thread;
		current_thread = list_entry(current_pos->list.next, struct thread, list);
		current_pos = current_thread;
		current_pos->must_be_switched = false;
		TRACE("%d %d\n", prev_thread->id, current_thread->id);
		while  (preemption_count != 1) {
		}
		preemption_count--;
		context_switch(&prev_thread->thread_context, &current_thread->thread_context);
	}
}

void scheduler_tick(uint32_t id) {
	static int counter = 0;
	TRACE("\nTick\n");
	if (counter >= SCHEDULER_INTERVAL) {
		counter = 0;
		current_thread->must_be_switched = true;
	}
	counter++;
}

void scheduler_add(struct thread *added_thread) {
//	INIT_LIST_HEAD(&added_thread->list);
	list_add(&added_thread->list, list_begin->list.prev);
}

int scheduler_remove(struct thread *removed_thread) {
	if (removed_thread == NULL || removed_thread == list_begin) {
		return -EINVAL;
	}
	scheduler_lock();
	if (current_thread == removed_thread) {
		current_pos = list_entry(current_thread->list.prev, struct thread, list);
		current_thread->must_be_switched = true;
	}
	list_del(&removed_thread->list);
	scheduler_unlock();
	return 0;
}
