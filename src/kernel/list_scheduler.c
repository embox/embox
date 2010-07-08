/**
 * @file
 * @brief Implementation of methods in scheduler_base.h
 *
 * @date 1.07.2010
 * @author Avdyukhin Dmitry
 */

#ifndef _LIST_SCHEDULER_
#define _LIST_SCHEDULER_

#include <lib/list.h>
#include <kernel/scheduler_base.h>

/**
 * List item, pointing at the beginning of the
 * 	list of running threads.
 */
static struct list_head *list_head_run;

void _scheduler_start(void) {
	list_for_each_entry(current_thread, list_head_run, sched_list) {
		TRACE("%d ", current_thread->id);
	}
}

void _scheduler_add(struct thread *added_thread) {
	list_add_tail(&added_thread->sched_list, list_head_run);
}

void _scheduler_init(void) {
	INIT_LIST_HEAD(&idle_thread->sched_list);
	list_head_run = &idle_thread->sched_list;
}

struct thread *_scheduler_next(struct thread *prev_thread) {
	if (prev_thread->sched_list.next == NULL) {
		return idle_thread;
	}
	return list_entry(prev_thread->sched_list.next, struct thread, sched_list);
}

void _scheduler_remove(struct thread *removed_thread) {
	if (removed_thread->sched_list.next != NULL) {
		list_del(&removed_thread->sched_list);
	}
}

#endif
