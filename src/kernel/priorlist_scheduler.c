/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 * @author Darya Dzendzik
 */

#include <errno.h>
#include <stdlib.h>
#include <lib/list.h>

#include <kernel/scheduler.h>
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


/*My comment
typedef struct thread1 {
	int id;
} thread_t;


struct thread1 {
  	int id;
	};

typedref struct thread1 thread_t;*/

/**
 * structure thread in structure priority
 */
typedef struct thread_head {
	struct list_head *next;
	struct list_head *prev;
	struct thread *thr;
} thread_head_t;

/**
 * structure priority in list
 */
typedef struct priority_head {
	struct list_head *next;
	struct list_head *prev;
	int priority_id;
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
#define priority_head          (&priority_pool[0])
static int free_pool_priority = 1; // need for allocation
static priority_head_t *cur_prior;

static thread_head_t thread_head_pool[NUMBER_THREAD];

/**
 * this is the empty list
 */
static LIST_HEAD(free_threads_list_head);

/**
 * empty list = empty head of lest
 */
#define free_threads_list (&free_threads_list_head)

/**
 * allocation head of thread in memory
 * MAST:
 * 1. alloc memory
 * 2. made structure
 */
static thread_head_t *alloc_thread_head(thread_t *thr) {
	thread_head_t *head;
	if (list_empty(free_threads_list)) {
		return NULL;//% - зачем это? Почему так надо???
	}
	head = (thread_head_t *) free_threads_list->next;
	list_del((struct list_head *) free_threads_list->next);
	head->thr;
	return head;
	//MAGIC!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1!!!!!!!!!!!!!!!!

}

static priority_head_t * alloc_priority(void){
	return(&priority_pool[free_pool_priority++]);
}


/**
 * delete head of thread
 */
static void free_thread_head(thread_head_t *head) {
	list_add((struct list_head *) head, free_threads_list);
}

/*why*/
#if 0
static void hold_priority(thread *pthread, int priority) {
	int i;
	list_for_each(pos, head) {
		if (priority_pool.priority_id = priority) {
			break;
		}

		list_add(*pthrread);
	}
	if (!priority_pool.priority_id = priority) {
		priority_pool.add_id(priority);
	}
	priority_pool.thread_plist.list_add(thread * pthread);
}
#endif

/**
 * it's need for blocking
 */
static int preemption_count = 1;
static int changed_max_priority = 0;

/**
 *adding new priority structure to priority list
 *we mast:
 *1. allocation memory
 *2. add priority
 *3. sort
 */
static void add_new_priority(thread_head_t *thr_head, int priority) {
	/*allocate new  priority header*/
	priority_head_t *new_priority = alloc_priority();
	priority_head_t *current_pr = priority_head->next;
	priority_head_t *help;
	while (current_pr->priority_id>priority)
		current_pr = current_pr->next;

	help = current_pr->next;
	new_priority->next = help;
	new_priority->prev = current_pr;
	help->prev = new_priority;
	current_pr->next = new_priority;

	new_priority->priority_id = priority;
	new_priority->thread_list = thr_head;
	return;

	/*set pointer to first thread header*/
}

/**
 * get list by priority
 */
static thread_head_t * get_list_by_priority(int priority) {
	struct list_head *p;

	if (idle_thread == priority) {    // TREAD_PRIORITY_IDLE -> idle_thread
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
static void add_thread_by_priority(struct thread*thr, int priority) {
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

/** List item, pointing at begin of the list. */
static struct list_head *list_begin;

/**
 * filling list of priority
 */
int scheduler_init(void) {
	/*we initialize zero element we always have idle_thread */
	priority_pool[0].priority_id = idle_thread;   // TREAD_PRIORITY_IDLE -> idle_thread
	priority_pool[0].thread_list = 0;
	current_thread = idle_thread;

	current_thread->reschedule = false;
	return(0);
}

/**
 * Is regularly called to show that current thread to be changed.
 * @param id nothing significant
 */
static void scheduler_tick(uint32_t id) {
	TRACE("\nTick\n");
	current_thread->reschedule = true;
}
/**
 * I think here scheduler start to work
 * I think it work's right
 */
void scheduler_start(void) {

	/* Redundant thread, which will never work. */
	struct thread redundant_thread;

	TRACE("\nStart scheduler\n");
	list_for_each_entry(current_thread, list_begin, sched_list) {
		TRACE("%d ", current_thread->id);
	}
	set_timer(THREADS_TIMER_ID, THREADS_TIMER_INTERVAL, scheduler_tick);

	ipl_disable();
	preemption_count--;
	context_switch(&redundant_thread.context, &idle_thread->context);
	/* NOTREACHED */
}

/**
 * blocking
 */
void scheduler_lock(void) {
	preemption_count++;
}

/**
 * unblocking
 */
void scheduler_unlock(void) {
	preemption_count--;
	if (preemption_count == 0 && current_thread->reschedule) {
		scheduler_dispatch();
	}
}

/**
 * Move current_thread pointer to the next thread.
 * @param prev_thread thread, which have worked just now.
 */

static void thread_move_next(struct thread *prev_thread) {
	//free current thread
	cur_prior->thread_list = cur_prior->thread_list->next;

	cur_prior = ((priority_head_t *)(priority_head->next));
	current_thread  = ((priority_head_t *)(priority_head->next))->thread_list;

	if (current_thread->state == THREAD_STATE_ZOMBIE) {
		list_del(&current_thread->sched_list);
		current_thread
			= list_entry(prev_thread->sched_list.next, struct thread, sched_list);
	}
	current_thread->reschedule = false;
}


/**
 * main function - switch thread
 */
void scheduler_dispatch(void) {
	/* Thread, which have worked just now. */

	struct thread *prev_thread;
	ipl_t ipl;

	if (preemption_count == 0 && current_thread->reschedule) {
		preemption_count++;
		thread_move_next(current_thread);
		TRACE("Switching from %d to %d\n", prev_thread->id, current_thread->id);
		ipl = ipl_save();
		preemption_count--;
		context_switch(&prev_thread->context, &current_thread->context);
		ipl_restore(ipl);
	}
}

void scheduler_add(struct thread *added_thread) {
		add_thread_by_priority(added_thread, added_thread->priority);
}

/**
 * TODO посмотреть и разбратся - так или не так...
 */
int scheduler_remove(struct thread *removed_thread) {
	if (removed_thread == NULL || removed_thread == idle_thread) {
		return -EINVAL;
	}
	scheduler_lock();
	removed_thread->reschedule = true;
	scheduler_unlock();
	return 0;
}
