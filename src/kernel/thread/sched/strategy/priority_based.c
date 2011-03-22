/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Darya Dzendzik
 */

#include <assert.h>
#include <stdbool.h>

#include <lib/list.h>
#include <util/array.h>
#include <kernel/thread.h>
#include <kernel/scheduler_base.h>

/**
 * Structure priority in list.
 */
struct run_thread_list {
	/** Link within sorted priorities list. */
	struct list_head priority_link;

	/** List of threads with given priority. */
	struct list_head thread_list;
};

/**
 * Thread that runs now.
 */
static struct thread *current_thread;

/**
 * Array of thread list ordered by priority
 * (index is priority of each thread contained in list).
 */
static struct run_thread_list priorities[THREAD_PRIORITY_TOTAL];

/**
 * Head of run_queue is executable thread with maximal of existing priorities.
 * Contains idle_thread (last).
 */
static struct list_head run_queue;

/**
 * TODO --Alina
 * @return
 */
static struct thread *run_peek(void) {
	struct run_thread_list *priority;

	if (list_empty(&run_queue)){
		return NULL;
	}

	priority = list_entry(run_queue.next, struct run_thread_list, priority_link);
	if (list_empty(&priority->thread_list)){
		return NULL;
	}

	return list_entry(priority->thread_list.next, struct thread, sched_list);
}

/**
 * TODO --Alina
 * @return
 */
static struct thread *run_dequeue(void) {
	struct thread *thread = run_peek();

	if(!thread) {
		return NULL;
	}

	list_del_init(&thread->sched_list);
	return thread;
}

/**
 * Insert priority_link to run_queue if threads with given priority don't exist.
 */
static void run_insert_priority(struct run_thread_list *priority) {
	struct run_thread_list *next_priority;

	assert(list_empty(&priority->priority_link));

	if (list_empty(&run_queue)) {
		list_add(&priority->priority_link, &run_queue);
		return;
	}

	list_for_each_entry(next_priority, &run_queue, priority_link) {
		if (next_priority > priority) {
			break;
		}
	}

	list_add_tail(&priority->priority_link, &next_priority->priority_link);
}
/**
 * TODO --Alina
 * @param thread
 */
static void run_enqueue(struct thread *thread) {
	struct run_thread_list *priority = priorities + thread->priority;
	if (list_empty(&priority->priority_link)){
		run_insert_priority(priority);
	}
	list_add_tail(&thread->sched_list, &priority->thread_list);
}
/**
 * TODO --Alina
 * @param thread
 */
static void run_push(struct thread *thread) {
	struct run_thread_list *priority = priorities + thread->priority;
	if (list_empty(&priority->priority_link)){
		run_insert_priority(priority);
	}
	list_add(&thread->sched_list, &priority->thread_list);
}
/**
 * TODO --Alina
 * @return
 */
struct thread *_scheduler_current(void) {
	return current_thread;
}

/**
 * Adds thread to existing thread list
 * or creates a new priority list and adds thread to this list.
 */
void _scheduler_add(struct thread *thread) {
	struct run_thread_list *priority = priorities + thread->priority;

	if (list_empty(&priority->thread_list)) {
		run_insert_priority(priority);
	}

	run_enqueue(thread);

	// XXX
	if (!current_thread) {
		current_thread = thread;
	}
}

/**
 * Move next_thread pointer to the next thread.
 * @param prev_thread thread which have worked just now.
 */
struct thread *_scheduler_next(struct thread *prev_thread) {
	struct run_thread_list *priority = priorities + prev_thread->priority;
	struct thread *next;

	assert(current_thread != NULL);

	if (current_thread->state == THREAD_STATE_RUN) {
		if (!(next = run_peek())) {
			return current_thread;
		}
		if (current_thread->priority > next->priority) {
			/* Preemption */
			run_push(current_thread);
		} else {
			/* Quantum expiration */
			run_enqueue(current_thread);
		}
	} else {
		if (list_empty(&priority->thread_list)) {
			list_del_init(&priority->priority_link);
		}
	}

	if (!(next = run_dequeue())) {
		return current_thread;
	}

	return (current_thread = next);
}
/**
 * Delete thread from run_queue.
 * @param thread
 *  removed thread
 */
void _scheduler_remove(struct thread *thread) {
	struct run_thread_list *priority = priorities + thread->priority;
	struct list_head *same_priority_list = &priority->thread_list;

	if (thread == current_thread) {
		current_thread->reschedule = true;
		current_thread->state = THREAD_STATE_ZOMBIE;
		return;
	}

	list_del_init(&thread->sched_list);

	if (list_empty(&thread->sched_list)) {
		/* Remove link on list of threads with given priority */
		list_del_init(&priority->priority_link);
	}

	if (list_empty(same_priority_list)) {
		list_del_init(same_priority_list);
	}
}

/**
 * Scheduler start to work.
 */
void _scheduler_start(void) {
	/* Nothing to do. */
}

/**
 * Scheduler has finished its work.
 */
void _scheduler_stop(void) {
	/* Nothing to do. */
}

/**
 * Initializes list of priority.
 */
void _scheduler_init(void) {
	struct run_thread_list *ptr;

	INIT_LIST_HEAD(&run_queue);
	array_static_foreach_ptr(ptr, priorities) {
		INIT_LIST_HEAD(&(ptr->thread_list));
		INIT_LIST_HEAD(&(ptr->priority_link));
	}

	current_thread = NULL;
}
