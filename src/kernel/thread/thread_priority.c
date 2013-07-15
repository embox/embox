/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 */

#include <kernel/thread.h>
#include <kernel/thread/state.h>
#include <kernel/thread/thread_priority.h>
#include <kernel/task.h>

int thread_priority_set(struct thread *t, __thread_priority_t new_priority) {
	assert(t);

	if(t->state == __THREAD_STATE_INIT) {
		t->thread_priority.initial_priority = get_sched_priority(t->task->priority, t->priority);
		t->thread_priority.sched_priority = t->thread_priority.initial_priority;
	}
	return 0;
}

__thread_priority_t thread_priority_get(struct thread *t) {
	assert(t);

	return t->thread_priority.sched_priority;
}

__thread_priority_t thread_priority_inherit(struct thread *t, __thread_priority_t priority) {
	assert(t);

	if(priority > t->thread_priority.sched_priority) {
		t->thread_priority.sched_priority = priority;
	}

	return t->thread_priority.sched_priority;
}

__thread_priority_t thread_priority_reverse(struct thread *t) {
	assert(t);

	t->thread_priority.sched_priority = t->thread_priority.initial_priority;

	return t->thread_priority.sched_priority;
}
