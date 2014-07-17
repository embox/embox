/**
 * @file
 *
 * @date March 30, 2013
 * @author: Andrey Kokorev
 */

#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>
#include <kernel/runnable/runnable_priority.h>

#define prior_field(field)   lwt->runnable.sched_attr.thread_priority.field

int lwthread_priority_init(struct lthread *lwt, sched_priority_t new_priority) {
	assert(lwt);

	runnable_priority_init(&(lwt->runnable), new_priority);

	return 0;
}

int lwthread_priority_set(struct lthread *lwt, sched_priority_t new_priority) {
	assert(lwt);

	runnable_priority_set(&(lwt->runnable), new_priority);

	return 0;
}

sched_priority_t lwthread_priority_get(struct lthread *lwt) {
	assert(lwt);

	return runnable_priority_get(&(lwt->runnable));
}

sched_priority_t lwthread_priority_inherit(struct lthread *lwt,
		sched_priority_t priority) {
	assert(lwt);

	return runnable_priority_inherit(&(lwt->runnable), priority);
}

sched_priority_t lwthread_priority_reverse(struct lthread *lwt) {
	assert(lwt);

	return runnable_priority_reverse(&(lwt->runnable));
}
