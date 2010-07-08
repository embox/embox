/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Darya Dzendzik
 */

#include <errno.h>
#include <stdlib.h>
#include <lib/list.h>
#include <kernel/thread.h>

#include <kernel/scheduler.h>
#include <kernel/timer.h>
#include <hal/context.h>
#include <hal/ipl.h>
#include <embox/unit.h>
#include <kernel/scheduler_base.h>

/** Timer, which calls scheduler_tick. */
#define THREADS_TIMER_ID 17

/** Interval, what scheduler_tick is called in. */
#define THREADS_TIMER_INTERVAL 100

/**
 * structure priority in list
 */
typedef struct priority_head {
	struct list_head *next;
	struct list_head *prev;
	thread_id_t priority_id;
	thread_head_t *thread_list;
} priority_head_t;

/**
 * define max priority 16
 * idle thread has priority = 0
 * define max number thread 16
 */
#define MAX_PRIORITY 0x10
#define THREAD_PRIOR6ITY_IDLE 0
#define NUMBER_THREAD 0x10
/**
 * Pools for thread and priority
 */
static priority_head_t priority_pool[MAX_PRIORITY];
/*head of priority list - null priority, next - max priority*/
static priority_head_t *priority_head = (priority_pool);
static priority_head_t *cur_prior;

static thread_head_t thread_head_pool[NUMBER_THREAD];

static LIST_HEAD(free_threads_list1);
#define free_threads_list (&free_threads_list1)
/**
 * allocation head of thread in memory
 * free_thresd_list - pool
 * MUST:
 * 1. alloc memory from pool
 * 2. made structure
 */
static thread_head_t *alloc_thread_head(thread_t *thr) {
	thread_head_t *head;
	if (list_empty((struct list_head *)free_threads_list)) {
		return NULL;
	}
	head = (thread_head_t *) free_threads_list->next;
	list_del((struct list_head *) free_threads_list->next);
	head->thr = thr;
	thr->thread_head = head;
	return head;

}

static priority_head_t * alloc_priority(thread_priority_t priority) {
	return (&priority_pool[priority]);
}
/**
 * delete head of thread
 */
static void remov_thread_head(thread_head_t *removed_thread) {
	list_add((struct list_head *) removed_thread, (struct list_head *) free_threads_list);
	if ((struct list_head *)&removed_thread == removed_thread->next){
		list_del((struct list_head *)&priority_pool[removed_thread->thr->priority]);
		return;
	}
	if (priority_pool[removed_thread->thr->priority].thread_list == removed_thread){
	priority_pool[removed_thread->thr->priority].thread_list = (thread_head_t *)
			priority_pool[removed_thread->thr->priority].thread_list->next;
	}
	list_del((struct list_head *) removed_thread);
}

/**
 * it's need for blocking
 */
//static int preemption_count = 1;

/**
 *adding new priority structure to priority list
 *we mast:
 *1. allocation memory
 *2. add priority
 *3. sort
 */
static void add_new_priority(thread_head_t *thr_head, thread_priority_t priority) {
	/*allocate new  priority header*/
	priority_head_t *new_priority = alloc_priority(priority);
	priority_head_t *current_pr = (priority_head_t *) priority_head->next;
	//priority_head_t *tmp;

	while (current_pr->priority_id > priority) {
		current_pr = (priority_head_t *) current_pr->next;
	}
	list_add((struct list_head *) new_priority, ((struct list_head *) current_pr)->prev);
	new_priority->priority_id = priority;
	new_priority->thread_list = thr_head;
	INIT_LIST_HEAD((struct list_head *)thr_head);
	//thr_head->next = ((struct list_head *) thr_head)->prev = (struct list_head *) thr_head;
	return;
}

/**
 * get list by priority
 */
static thread_head_t * get_list_by_priority(thread_priority_t priority) {
	struct list_head *p;

	if (idle_thread->priority == priority) { // TREAD_PRIORITY_IDLE -> idle_thread
		return priority_head->thread_list;
	}
	list_for_each(p, (struct list_head *)priority_head) {
		if (((priority_head_t *) p)->priority_id == priority) {
			return ((priority_head_t *) p)->thread_list;
		}
	}
	return NULL;
}

/**
 *adding thread with priority to thread list
 *or adding priority to priority list and thread to thread list
 */
static void add_thread_by_priority(struct thread*thr, thread_priority_t  priority) {
	thread_head_t *list_head;
	thread_head_t *thr_head;
	/*check - is priority?*/
	if (NULL == (thr_head = alloc_thread_head(thr))) {
		return; //empty
	}

	if (NULL == (list_head = get_list_by_priority(priority))) {
		/*adding first thread that priority to list*/
		add_new_priority(thr_head, priority);
		return;// No, add new priority
	}
	list_add((struct list_head *) thr_head, (struct list_head *) list_head); //Yes
}

/**
 * filling list of priority
 */
void _scheduler_init(void) {
	int i;
	for (i = 1; i < NUMBER_THREAD; i++) {
		list_add((struct list_head *) (&thread_head_pool[i]), (struct list_head *) free_threads_list);
	}

	/*we initialize zero element we always have idle_thread */
	INIT_LIST_HEAD((struct list_head *)priority_head);
	//priority_head->next = ((struct list_head *) priority_head)->prev =(struct list_head *) priority_head;
	priority_head->priority_id = 0;
	priority_head->thread_list = alloc_thread_head(idle_thread);
	INIT_LIST_HEAD((struct list_head *)priority_head->thread_list);

	cur_prior = priority_head;


}

/**
 * scheduler start to work
 */
void _scheduler_start(void) {
}

/**
 * Move current_thread pointer to the next thread.
 * @param prev_thread thread, which have worked just now.
 */
struct thread *_scheduler_next(struct thread *prev_thread) {
	//free current thread
	struct thread *current_thread;
	cur_prior->thread_list = (thread_head_t *) (cur_prior->thread_list->next);

	cur_prior = ((priority_head_t *) (priority_head->next));
	current_thread = (((priority_head_t *) (priority_head->next))->thread_list)->thr;

	if (current_thread->state == THREAD_STATE_ZOMBIE) {
		list_del(&current_thread->sched_list);
		current_thread
				= list_entry(prev_thread->sched_list.next, struct thread, sched_list);
	}
	current_thread->reschedule = false;
	return (current_thread);
}

void _scheduler_add(struct thread *added_thread) {
	add_thread_by_priority(added_thread, added_thread->priority);
}

/**
 * TODO watch...
 */
void _scheduler_remove(struct thread *removed_thread) {
	if (removed_thread == NULL || removed_thread == idle_thread) {
	return;
	}
	remov_thread_head(removed_thread->thread_head);
}
