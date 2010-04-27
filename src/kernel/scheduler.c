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
 * Stack for idle_thread.
 */
static char idle_thread_stack[THREAD_STACK_SIZE];
/**
 * List item, pointing at begin of the list.
 */
static struct thread_list *begin;
/**
 * List item, pointing at currently executed tread.
 */
static struct thread_list *current_pos;
/**
 * List item, pointing at tread, deleted from the list.
 */
struct thread_list *del_pos;
/**
 * Redundant thread, which will never work.
 */
static struct thread redundant_thread;

/**
 * Structure, describing list, containing threads.
 */
struct thread_list {
	struct thread_list *prev, *next;
	struct thread *data;
};

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
	begin = (struct thread_list *)malloc(sizeof(struct thread));
	current_pos = begin;
	begin->data = &idle_thread;
	begin->next = begin->prev = begin;
	current_thread = &idle_thread;
	prev_thread = &idle_thread;
	current_thread->must_be_switched = false;
}

void scheduler_start(void) {
	TRACE("\nStart scheduler\n");
	for (struct thread_list *aaa = begin->next; aaa != begin ;aaa = aaa->next) {
		TRACE("%d ", aaa->data->id);
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
//	assert(preemption_count == 0);
	if (preemption_count == 0 && current_thread->must_be_switched) {
		preemption_count++;
		prev_thread = current_thread;
		current_pos = current_pos->next;
		current_thread = current_pos->data;
		current_thread->must_be_switched = false;
		TRACE("%d %d\n", prev_thread->id, current_thread->id);
		preemption_count--;
		while  (preemption_count != 0) {
		}
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
	struct thread_list *new_list_item =
		(struct thread_list *)malloc(sizeof(struct thread));
	new_list_item->prev = begin->prev;
	new_list_item->next = begin;
	begin->prev->next = new_list_item;
	begin->prev = new_list_item;
	new_list_item->data = added_thread;
}

int scheduler_remove(struct thread *removed_thread) {
	if (removed_thread == NULL) {
		return -EINVAL;
	}
	scheduler_lock();
	for (del_pos = begin->next; del_pos != begin &&
		del_pos->data != removed_thread; del_pos = del_pos->next) {
	}
	if (del_pos == begin) {
		return -EINVAL;
	}
	del_pos->prev->next = del_pos->next;
	del_pos->next->prev = del_pos->prev;
	if (current_pos == del_pos) {
		assert(current_thread == removed_thread);
		current_pos = del_pos->prev;
		current_thread->must_be_switched = true;
	}
	free(del_pos);
	scheduler_unlock();
	return 0;
}
