/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 */

#include <kernel/thread.h>
#include <kernel/runnable/runnable_priority.h>

#define prior_field(field)   t->runnable.sched_attr.thread_priority.field

int thread_priority_init(struct thread *t, sched_priority_t new_priority) {
	assert(t);
	runnable_priority_init(&t->runnable, new_priority);
	return 0;
}

int thread_priority_set(struct thread *t, sched_priority_t new_priority) {
	assert(t);
	runnable_priority_set(&t->runnable, new_priority);
	return 0;
}

sched_priority_t thread_priority_get(struct thread *t) {
	assert(t);
	return runnable_priority_get(&t->runnable);
}

sched_priority_t thread_priority_inherit(struct thread *t,
		sched_priority_t priority) {
	assert(t);
	return runnable_priority_inherit(&t->runnable, priority);
}

sched_priority_t thread_priority_reverse(struct thread *t) {
	assert(t);
	return runnable_priority_reverse(&t->runnable);
}
