/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 */

#include <kernel/thread.h>
#include <hal/context.h>
#include <kernel/scheduler.h>
#include <errno.h>
#include <kernel/timer.h>
#include <stdlib.h>


#define THREAD_STACK_SIZE 0x1000
#define THREADS_TIMER_ID 2
#define THREADS_TIMER_TICKS 5
#define SCHEDULER_INTERVAL 50



/**
 * If it doesn't equal to zero, it means
 * that we are located in critical section
 * and cant's switch between threads.
 */
static int preemption_count;
/**
 * Thread, which makes nothing.
 * Is used to be working when there is no another process.
 */
static struct thread idle_thread;
/**
 * Thread, which works now.
 */
static struct thread *current_thread;
/**
 * Thread, which will be switched on.
 */
static struct thread *next_thread;
/**
 * Stack for idle_thread.
 */
static char idle_thread_stack[THREAD_STACK_SIZE];
/**
 * Stack for idle_thread.
 */
static struct thread_list *begin;
/**
 * Stack for idle_thread.
 */
static struct thread_list *current_pos;

/**
 * Structure, describing list, containing threads.
 */
struct thread_list {
	struct thread_list *prev, *next;
	struct thread *data;
};

static void idle_run(void) {
	while (true) {
		TRACE("?");
	}
}

void scheduler_init(void) {
	preemption_count = 0;
	thread_create(&idle_thread, idle_run, idle_thread_stack
			+ sizeof(idle_thread_stack));
	idle_thread.must_be_switched = false;
	current_pos = begin =
		(struct thread_list *)malloc(sizeof(struct thread));
	begin->data = &idle_thread;
	begin->next = begin->prev = begin;
	next_thread = &idle_thread;
}

static struct context acontext;

void scheduler_start(void) {
	TRACE("\nStart scheduler\n");
	set_timer(THREADS_TIMER_ID, THREADS_TIMER_TICKS, scheduler_tick);
	context_switch(&acontext, &idle_thread.thread_context);
}


void scheduler_lock(void) {
	preemption_count++;
}

void scheduler_unlock(void) {
	if (--preemption_count == 0) {
		scheduler_dispatch();
	}
}

void scheduler_dispatch(void) {
	TRACE("\nDispatch is called\n");
	if (preemption_count == 0 && current_pos->data->must_be_switched) {
		current_thread = next_thread;
		current_pos = current_pos->next;
//		current_pos = current_pos;
		next_thread = current_pos->data;
		if (next_thread == NULL) {
			TRACE("\nNext thread is Null\n");
		}
		next_thread->must_be_switched = false;
//		set_timer(THREADS_TIMER_ID, THREADS_TIMER_TICKS, scheduler_tick);
		context_switch(&current_thread->thread_context, &next_thread->thread_context);
	}
}

void scheduler_tick(uint32_t id) {
	static int counter = 0;
	TRACE("\nScheduler tick happened\n");
	if (counter >= SCHEDULER_INTERVAL) {
		TRACE("\nTick happened\n");
		counter = 0;
		current_pos->data->must_be_switched = true;
		scheduler_dispatch();
		TRACE("\nTick ended\n");
	}
	counter++;
}

void scheduler_add (struct thread *added_thread) {
	struct thread_list *new_list_item =
		(struct thread_list *)malloc(sizeof(struct thread));
	new_list_item->prev = begin->prev;
	new_list_item->next = begin;
	begin->prev->next = new_list_item;
	begin->prev = new_list_item;
	new_list_item->data = added_thread;
}

int scheduler_remove (struct thread *removed_thread) {
	if (removed_thread == NULL) {
		return -EINVAL;
	}
	scheduler_lock();
	struct thread_list *del_pos;
	for (del_pos = begin->next; del_pos != begin &&
		del_pos->data != removed_thread; del_pos = del_pos->next) {
	}
	if (del_pos == begin) {
		return -EINVAL;
	}
	del_pos->prev->next = del_pos->next;
	del_pos->next->prev = del_pos->prev;
	free(del_pos);
	scheduler_unlock();
	return 0;
}
