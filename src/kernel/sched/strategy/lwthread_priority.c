/**
 * @file
 *
 * @date March 30, 2013
 * @author: Andrey Kokorev
 */

#include <kernel/lwthread/lwthread.h>
#include <kernel/lwthread/lwthread_priority.h>
#include <kernel/runnable/runnable_priority.h>

#define prior_field(field)   lwt->runnable.sched_attr.thread_priority.field

int lwthread_priority_init(struct lwthread *lwt, sched_priority_t new_priority) {
	assert(lwt);

	runnable_priority_init(&(lwt->runnable), new_priority);

	return 0;
}

int lwthread_priority_set(struct lwthread *lwt, sched_priority_t new_priority) {
	assert(lwt);

	runnable_priority_set(&(lwt->runnable), new_priority);

	return 0;
}

sched_priority_t lwthread_priority_get(struct lwthread *lwt) {
	assert(lwt);

	return runnable_priority_get(&(lwt->runnable));
}

sched_priority_t lwthread_priority_inherit(struct lwthread *lwt,
		sched_priority_t priority) {
	assert(lwt);

	return runnable_priority_inherit(&(lwt->runnable), priority);
}

sched_priority_t lwthread_priority_reverse(struct lwthread *lwt) {
	assert(lwt);

	return runnable_priority_reverse(&(lwt->runnable));
}
