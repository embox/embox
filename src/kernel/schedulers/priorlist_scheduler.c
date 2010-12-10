/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Darya Dzendzik
 */

#include <lib/list.h>
#include <kernel/thread.h>
#include <kernel/scheduler_base.h>

/** Timer, which calls scheduler_tick. */
#define THREADS_TIMER_ID 17

/** Interval, what scheduler_tick is called in. */
#define THREADS_TIMER_INTERVAL 100

/**
 * Structure priority in list.
 */
typedef struct priority_head {
	struct list_head priority_list;

	thread_id_t priority_id;
	/** List of threads with priority = priority_id. */
	struct list_head *thread_list;
} priority_head_t;

/**
 * Maximum priority equals to 256.
 */
#define MAX_PRIORITY 0x100
/**
 * Pools for thread and priority.
 */
static priority_head_t priority_pool[MAX_PRIORITY+1];
/**
 * Head of priority list - null priority , next - max priority.
 * Contains idle_thread.
 */
static priority_head_t *priority_head = priority_pool;
/* Priority_head, containing currently working thread. */
static priority_head_t *cur_prior;

/**
 * Allocates priority_head for chosen priority.
 */
static priority_head_t *alloc_priority(thread_priority_t priority) {
	return (priority_pool + priority);
}

/**
 * Add new priority_head to priority list.
 * 1. Allocate memory;
 * 2. Add priority_head in right place in list;
 * 3. Add thread in priority_head.
 */
static void add_new_priority(struct thread *thread, thread_priority_t priority) {
	/* Allocate new  priority_head. */
	priority_head_t *new_priority = alloc_priority(priority);
	priority_head_t *current_pr;

	list_for_each_entry(current_pr, &priority_head->priority_list, priority_list) {
		if (current_pr->priority_id > priority) {
			break;
		}
	}
	list_add(&new_priority->priority_list, &current_pr->priority_list);
	new_priority->priority_id = priority;
	new_priority->thread_list = &thread->sched_list;
	INIT_LIST_HEAD(&thread->sched_list);
	return;
}

/**
 * Get head on list of threads with chosen priority.
 */
static struct list_head *get_priority_list(thread_priority_t priority) {
	struct list_head *p;

	if (idle_thread->priority == priority) {
		return priority_head->thread_list;
	}
	list_for_each(p, (struct list_head *) priority_head) {
		if (((priority_head_t *) p)->priority_id == priority) {
			return ((priority_head_t *) p)->thread_list;
		}
	}
	return NULL;
}

/**
 * Adds thread with priority to thread list
 * or adds priority to priority list and thread to thread list.
 */
void _scheduler_add(struct thread *added_thread) {
	struct list_head *list_head;

	/* Find priority_head with corresponding priority_id. */
	list_head = get_priority_list(added_thread->priority);
	if (NULL == list_head) {
		add_new_priority(added_thread, added_thread->priority);
		return;
	}
	list_add(&added_thread->sched_list, list_head);
}

/**
 * Initialises list of priority.
 */
void _scheduler_init(void) {
	/* Initializes zero element (we always have idle_thread). */
	INIT_LIST_HEAD(&priority_head->priority_list);
	priority_head->priority_id = 0;
	priority_head->thread_list = &idle_thread->sched_list;
	INIT_LIST_HEAD((priority_head->thread_list));

	cur_prior = priority_head;
}

/**
 * scheduler start to work
 */
void _scheduler_start(void) {
	/* Nothing to do. */
}

void _scheduler_stop(void) {
	/* Nothing to do. */
}

/**
 * Move current_thread pointer to the next thread.
 * @param prev_thread thread, which have worked just now.
 */
struct thread *_scheduler_next(struct thread *prev_thread) {
	struct thread *current_thread;

	cur_prior = (priority_head_t *) priority_head->priority_list.next;
	cur_prior->thread_list = cur_prior->thread_list->next;
	current_thread = list_entry(cur_prior->thread_list, struct thread, sched_list);
	current_thread->reschedule = false;
	return current_thread;
}

/**
 * Delete thread from list in priorlist_head.
 * If thread was the only with its priority, delete corresponding priorlist_head.
 */
void _scheduler_remove(struct thread *removed_thread) {
	size_t pri;
	if (removed_thread->sched_list.next == 0) {
		return;
	}
	pri = removed_thread->priority;
	/* Check if removed one was the only thread with its priority. */
	if (list_empty(&removed_thread->sched_list)) {
		list_del((struct list_head *)&priority_pool[pri]);
		removed_thread->sched_list.next = 0;
	} else {
		/* Check if current thread_list of
		   priority equals to removed thread. */
		if (priority_pool[pri].thread_list
				== &removed_thread->sched_list) {
			priority_pool[pri].thread_list
				= priority_pool[pri].thread_list->next;
		}
		/* Now we can delete thread from list. */
		list_del(&removed_thread->sched_list);
	}
}
