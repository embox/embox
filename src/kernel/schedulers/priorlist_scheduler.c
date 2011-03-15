/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Darya Dzendzik
 */

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
 * Insert priority_link to run_queue if threads with given priority don't exist.
 */
static void insert_priority(struct run_thread_list *priority) {
	struct run_thread_list *next_priority;

	if (!list_empty(&priority->thread_list)) {
		return;
	}

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
 * Adds thread to existing thread list
 * or creates a new priority list and adds thread to this list.
 */
void _scheduler_add(struct thread *added_thread) {
	struct run_thread_list *priority = priorities + added_thread->priority;

	if (list_empty(&priority->thread_list)) {
		insert_priority(priority);
	}

	list_add(&added_thread->sched_list, &priority->thread_list);
}

/**
 * Initialises list of priority.
 */
void _scheduler_init(void) {
	struct run_thread_list *ptr;

	INIT_LIST_HEAD(&run_queue);
	array_static_foreach_ptr(ptr, priorities) {
		INIT_LIST_HEAD(&(ptr->thread_list));
		INIT_LIST_HEAD(&(ptr->priority_link));
	}

	_scheduler_add(idle_thread);
}

/**
 * Scheduler start to work
 */
void _scheduler_start(void) {
	/* Nothing to do. */
}

void _scheduler_stop(void) {
	/* Nothing to do. */
}

/**
 * Move current_thread pointer to the next thread.
 * @param prev_thread thread which have worked just now.
 */
struct thread *_scheduler_next(struct thread *prev_thread) {
	struct thread *current_thread;
	struct run_thread_list *priority = priorities + prev_thread->priority;

	if (prev_thread == idle_thread) {
		/**
		 * Idle_thread cannot be removed from run_queue
		 * and should be the last executable thread.
		 */
		return prev_thread;
	}

	list_del_init(&prev_thread->sched_list);

	if (list_empty(&priority->thread_list)) {
		list_del_init(&priority->priority_link);
		current_thread = list_entry(run_queue.next, struct thread, sched_list);
	} else {
		current_thread
				= list_entry((priority->thread_list).next, struct thread, sched_list);
		list_add_tail(&prev_thread->sched_list, &priority->thread_list);
	}

	return current_thread;
}

/**
 * Delete thread from list in priorlist_head.
 * If thread was the only with its priority, delete corresponding priorlist_head.
 */
void _scheduler_remove(struct thread *thread) {
	struct run_thread_list *priority;
	struct list_head *same_priority_list = &priority->thread_list;

	if (thread->state != THREAD_STATE_RUN) {
		list_del_init(&thread->sched_list);

		if (list_empty(&thread->sched_list)) {
			/** Remove link on list of threads with given priority */
			list_del_init(&priority->priority_link);
		}

	} else if (list_empty(same_priority_list)) {
		list_del_init(same_priority_list);

	}
}
