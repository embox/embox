/**
 * @file
 * @brief Implementation of methods in scheduler_base.h
 *
 * @date 1.07.2010
 * @author Avdyukhin Dmitry
 */

#include <lib/list.h>
#include <stdbool.h>

#include <kernel/thread/sched_policy.h>

/**
 * List item, pointing at the beginning of the list of running threads.
 */
static struct list_head run_q;

static struct thread *current_thread;

struct thread *sched_policy_current(void) {
	return current_thread;
}

int sched_policy_init(struct thread *current, struct thread *idle) {
	INIT_LIST_HEAD(&run_q);

	current_thread = current;

	sched_policy_start(idle);

	return 0;
}

struct thread *sched_policy_switch(struct thread *t) {
	if (t == current_thread) {
		list_add_tail(&run_q, &current_thread->sched_list);
		current_thread = list_entry(run_q.next, struct thread, sched_list);
		return current_thread;
	}

	return list_entry(t->sched_list.next, struct thread, sched_list);
}

bool sched_policy_start(struct thread *t) {
	list_add_tail(&run_q, &t->sched_list);
	return false;
}

bool sched_policy_stop(struct thread *t) {
	if (t == current_thread) {
		return true;
	}
	if (list_empty(&run_q)) {
		return false;
	}

	list_del(&t->sched_list);
	return false;
}

