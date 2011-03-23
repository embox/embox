/**
 * @file
 * @brief Implementation of methods in scheduler_base.h
 *
 * @date 1.07.2010
 * @author Avdyukhin Dmitry
 */

#include <lib/list.h>
#include <kernel/thread/sched_policy.h>

/**
 * List item, pointing at the beginning of the
 * 	list of running threads.
 */
static struct list_head *list_head_run;

void sched_policy_init(void) {
	INIT_LIST_HEAD(&idle_thread->sched_list);
	list_head_run = &idle_thread->sched_list;
}

void sched_policy_start(void) {
#ifdef CONFIG_DEBUG_SCHEDULER
	TRACE("Added threads: ");
	list_for_each_entry(current_thread, list_head_run, sched_list) {
		TRACE("%d ", current_thread->id);
	}
#endif
}

void sched_policy_stop(void) {
	/* Nothing to do. */
}

bool sched_policy_add(struct thread *added_thread) {
	list_add_tail(&added_thread->sched_list, list_head_run);
	return false;
}

struct thread *sched_policy_switch(struct thread *prev_thread) {
	if (prev_thread->sched_list.next == NULL) {
		return list_entry(idle_thread->sched_list.next,
			struct thread, sched_list);
	}
	return list_entry(prev_thread->sched_list.next,
		struct thread, sched_list);
}

bool sched_policy_remove(struct thread *removed_thread) {
	if (removed_thread->sched_list.next != NULL) {
		list_del(&removed_thread->sched_list);
	}
	return false;
}

