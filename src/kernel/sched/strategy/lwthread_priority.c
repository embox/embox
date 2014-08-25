/**
 * @file
 *
 * @date March 30, 2013
 * @author: Andrey Kokorev
 */

#include <kernel/lthread/lthread.h>
#include <kernel/sched/schedee_priority.h>

#define prior_field(field)   lt->schedee.thread_priority.field

int lthread_priority_init(struct lthread *lt, sched_priority_t new_priority) {
	assert(lt);
	schedee_priority_init(&lt->schedee, new_priority);
	return 0;
}

int lthread_priority_set(struct lthread *lt, sched_priority_t new_priority) {
	assert(lt);
	schedee_priority_set(&lt->schedee, new_priority);
	return 0;
}

sched_priority_t lthread_priority_get(struct lthread *lt) {
	assert(lt);
	return schedee_priority_get(&lt->schedee);
}

sched_priority_t lthread_priority_inherit(struct lthread *lt,
		sched_priority_t priority) {
	assert(lt);
	return schedee_priority_inherit(&lt->schedee, priority);
}

sched_priority_t lthread_priority_reverse(struct lthread *lt) {
	assert(lt);
	return schedee_priority_reverse(&lt->schedee);
}
