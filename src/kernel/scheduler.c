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
static int preemption_count = 1;

/** List item, pointing at begin of the list. */
static struct list_head *list_begin_sched;
#if 0
/** List item, pointing at begin of the (waiting) list. */
static struct list_head *list_begin_wait;
/** List item, pointing at begin of the (sleeping) list. */
static struct list_head *list_begin_sleep;
#endif

int scheduler_init(void) {
	INIT_LIST_HEAD(&idle_thread->sched_list);
	current_thread = idle_thread;
	list_begin_sched = &idle_thread->sched_list;
#if 0
	list_begin_wait = &idle_thread->wait_list;
#endif
	current_thread->reschedule = false;
	return 0;
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
	TRACE("\nStart scheduler\n");
	list_for_each_entry(current_thread, list_begin_sched, sched_list) {
		TRACE("%d ", current_thread->id);
	}
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
static void thread_move_next(struct thread *prev_thread) {
	current_thread = list_entry(prev_thread->sched_list.next, struct thread, sched_list);
	if (current_thread->state == THREAD_STATE_ZOMBIE) {
		list_del(&current_thread->sched_list);
		current_thread = list_entry(prev_thread->sched_list.next, struct thread, sched_list);
	}
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
		TRACE("Switching from %d to %d\n", prev_thread->id, current_thread->id);

		ipl = ipl_save();
		preemption_count--;
		context_switch(&prev_thread->context, &current_thread->context);
		ipl_restore(ipl);
	}
}

void scheduler_add(struct thread *added_thread) {
	list_add_tail(&added_thread->sched_list, list_begin_sched);
}

int scheduler_remove(struct thread *removed_thread) {
	if (removed_thread == NULL || removed_thread == idle_thread) {
		return -EINVAL;
	}
	scheduler_lock();
	if (removed_thread->sched_list.next != NULL) {
		list_del(&removed_thread->sched_list);
	}
	scheduler_unlock();
	return 0;
}

int scheduler_convar_wait(struct mutex *added_mutex, struct condition_variable *variable) {
#if 0
	if (added_mutex == NULL || added_mutex == idle_mutex) {
		return -EINVAL;
	}
	&added_mutex->bound_thread->state = THREAD_STATE_SLEEP;
	list_add_tail(&added_mutex->sleeped_thread_list, variable->list_begin_convar);
	mutex_unlock(added_mutex);
	return 0;
#endif
}

void scheduler_convar_signal(struct condition_variable *variable) {
#if 0
	struct mutex trans_mutex;
	struct list_head *convar_list;
	convar_list = &variable->list_begin_convar;
	while (convar_list->next != NULL) {
		convar_list = convar_list->next;
		trans_mutex = list_entry(convar_list, struct mutex, sleeped_thread_list);
		list_del(&trans_mutex->sleeped_thread_list);
		&trans_mutex->bound_thread->state = THREAD_STATE_RUN;
		mutex_lock(trans_mutex);
	}
#endif
}
