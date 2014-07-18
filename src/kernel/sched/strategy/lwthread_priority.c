/**
 * @file
 *
 * @date March 30, 2013
 * @author: Andrey Kokorev
 */

#include <kernel/lthread/lthread.h>
#include <kernel/runnable/runnable_priority.h>

#define prior_field(field)   lt->runnable.sched_attr.thread_priority.field

int lthread_priority_init(struct lthread *lt, sched_priority_t new_priority) {
	assert(lt);
	runnable_priority_init(&lt->runnable, new_priority);
	return 0;
}

int lthread_priority_set(struct lthread *lt, sched_priority_t new_priority) {
	assert(lt);
	runnable_priority_set(&lt->runnable, new_priority);
	return 0;
}

sched_priority_t lthread_priority_get(struct lthread *lt) {
	assert(lt);
	return runnable_priority_get(&lt->runnable);
}

sched_priority_t lthread_priority_inherit(struct lthread *lt,
		sched_priority_t priority) {
	assert(lt);
	return runnable_priority_inherit(&lt->runnable, priority);
}

sched_priority_t lthread_priority_reverse(struct lthread *lt) {
	assert(lt);
	return runnable_priority_reverse(&lt->runnable);
}
