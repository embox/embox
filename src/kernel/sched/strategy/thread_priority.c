/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 */

#include <kernel/thread.h>
#include <kernel/sched/schedee_priority.h>

#define prior_field(field)   s->schedee.thread_priority.field

int thread_priority_init(struct thread *s, sched_priority_t new_priority) {
	assert(s);
	schedee_priority_init(&s->schedee, new_priority);
	return 0;
}

int thread_priority_set(struct thread *s, sched_priority_t new_priority) {
	assert(s);
	schedee_priority_set(&s->schedee, new_priority);
	return 0;
}

sched_priority_t thread_priority_get(struct thread *s) {
	assert(s);
	return schedee_priority_get(&s->schedee);
}

sched_priority_t thread_priority_inherit(struct thread *s,
		sched_priority_t priority) {
	assert(s);
	return schedee_priority_inherit(&s->schedee, priority);
}

sched_priority_t thread_priority_reverse(struct thread *s) {
	assert(s);
	return schedee_priority_reverse(&s->schedee);
}
