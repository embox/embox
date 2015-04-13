/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.08.2014
 */

#ifndef THREAD_PRIORITY_PRIV_H_
#define THREAD_PRIORITY_PRIV_H_

#include <kernel/thread/types.h>

static inline int thread_priority_init(struct thread *t, sched_priority_t new_priority) {
	schedee_priority_init(&t->schedee.priority, new_priority);
	return 0;
}

static inline int thread_priority_set(struct thread *t, sched_priority_t new_priority) {
	schedee_priority_set(&t->schedee.priority, new_priority);
	return 0;
}

static inline sched_priority_t thread_priority_get(struct thread *t) {
	return schedee_priority_get(&t->schedee.priority);
}

static inline sched_priority_t thread_priority_inherit(struct thread *t,
		sched_priority_t priority) {
	return schedee_priority_inherit(&t->schedee.priority, priority);
}

static inline sched_priority_t thread_priority_reverse(struct thread *t) {
	return schedee_priority_reverse(&t->schedee.priority);
}

#endif /* THREAD_PRIORITY_PRIV_H_ */
