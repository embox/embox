/*
 * @file
 *
 * @date Aug 1, 2013
 * @author: Anton Bondarev
 */

#ifndef SCHED_SIMPLE_PRIORITY_H_
#define SCHED_SIMPLE_PRIORITY_H_

struct runnable_priority {
};

typedef struct runnable_priority __runnable_priority_t;

static inline int runnable_priority_set(struct runnable *r,
		sched_priority_t new_priority) {
	return 0;
}

static inline sched_priority_t runnable_priority_get(struct runnable *r) {
	return 0;
}

static inline sched_priority_t runnable_priority_inherit(struct runnable *r,
		sched_priority_t priority) {
	return 0;
}

static inline sched_priority_t runnable_priority_reverse(struct runnable *r) {
	return 0;
}


#endif /* SCHED_SIMPLE_PRIORITY_H_ */
