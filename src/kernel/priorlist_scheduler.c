/**
 * @file
 * @brief Implementation of methods in scheduler.h
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 * @author Daria Dzendzik
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
typedef struct thread1 {
	int id;
} thread_t;

typedef struct thread_head {
	struct list_head *next;
	struct list_head *prev;
	thread_t *thr;
} thread_head_t;

typedef struct priority_head {
	struct list_head *next;
	struct list_head *prev;
	int priority_id;
	thread_head_t *thread_list;
} priority_head_t;

#define MAX_PRIORITY 0x10
#define THREAD_PRIORITY_IDLE 0

static priority_head_t priority_pool[MAX_PRIORITY];
#define priority_head          (&priority_pool[0])

static thread_head_t thread_head_pool[MAX_PRIORITY];
static LIST_HEAD(free_threads_list_head);
#define free_threads_list (&free_threads_list_head)

static thread_head_t *alloc_thread_head(thread_t *thr) {
	thread_head_t *head;
	if (list_empty(free_threads_list)) {
		return NULL;
	}
	head = (thread_head_t *) free_threads_list->next;
	list_del((struct list_head *) free_threads_list->next);
	head->thr;
	return head;
}

static void free_thread_head(thread_head_t *head) {
	list_add((struct list_head *) head, free_threads_list);
}

//static LIST_HEAD_INIT(active_head_tread);


/*
 static void add_id( int priority ) {
 while (priority_list.priority_id <= priority) {
 &priority_list = priority_list.prev;
 if ( priority_list.priority_id == priority )
 return;
 }
 list_head entry;
 entry.prev = priority_list;
 entry.next = priority_list.next;
 entry.next.prev = entry;
 entry.priority_id = priority;
 priority_list.next = entry;
 thread_plist =  NULL;
 }
 }
 */

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

static int preemption_count = 1;

static void add_new_priority(thread_head_t *thr_head, int priority) {
	/*allocate new  priority header*/
	/*set pointer to first thread header*/
}

static thread_head_t * get_list_by_priority(int priority) {
	struct list_head *p;

	if (THREAD_PRIORITY_IDLE == priority) {
		return priority_head->thread_list;
	}
	list_for_each(p, (struct list_head *)priority_head) {
		if (((priority_head_t *) p)->priority_id == priority) {
			return ((priority_head_t *) p)->thread_list;
		}
	}
	return NULL;
}

static void add_thread_by_priority(thread_t *thr, int priority) {
	thread_head_t *list_head;
	thread_head_t *thr_head;
	if (NULL == (thr_head = alloc_thread_head(thr))) {
		return;
	}

	if (NULL == (list_head = get_list_by_priority(priority))) {
		/*adding first thread that priority to list*/
		add_new_priority(thr_head, priority);
		return;
	}
	list_add((struct list_head *) thr_head, (struct list_head *) list_head);
}

/** List item, pointing at begin of the list. */
static struct list_head *list_begin;

void scheduler_init(void) {
	/*we initialize zero element we always have idle_thread */
	//LIST_HEAD(priority_list);
	priority_pool[0].priority_id = THREAD_PRIORITY_IDLE;
	priority_pool[0].thread_list = 0;
	current_thread = idle_thread;
	//hold_priority(idle_thread,0);

	current_thread->reschedule = false;
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

void scheduler_lock(void) {
	preemption_count++;
}

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
	current_thread
			= list_entry(prev_thread->sched_list.next, struct thread, sched_list);
	if (current_thread->state == THREAD_STATE_ZOMBIE) {
		list_del(&current_thread->sched_list);
		current_thread
				= list_entry(prev_thread->sched_list.next, struct thread, sched_list);
	}
	current_thread->reschedule = false;
}

void scheduler_dispatch(void) {
	/* Thread, which have worked just now. */

	/*struct thread *prev_thread;*/

	ipl_t ipl;

	if (preemption_count == 0 && current_thread->reschedule) {
		preemption_count++;
		/*prev_thread = current_thread;
		 thread_move_next(prev_thread);
		 TRACE("Switching from %d to %d\n", prev_thread->id, current_thread->id);*/

		current_thread = active_head_thread;

		ipl = ipl_save();
		preemption_count--;
		context_switch(&prev_thread->context, &current_thread->context);
		ipl_restore(ipl);
	}
}

void scheduler_add(struct thread *added_thread) {
	list_add_tail(&added_thread->sched_list, list_begin);
}

int scheduler_remove(struct thread *removed_thread) {
	if (removed_thread == NULL || removed_thread == idle_thread) {
		return -EINVAL;
	}
	scheduler_lock();
	removed_thread->reschedule = true;
	scheduler_unlock();
	return 0;
}
