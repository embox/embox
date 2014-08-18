/*
 * @file
 *
 * @date Aug 1, 2013
 * @author: Anton Bondarev
 */

#ifndef SCHED_SIMPLE_PRIORITY_H_
#define SCHED_SIMPLE_PRIORITY_H_

struct schedee_priority {
};

typedef struct schedee_priority __schedee_priority_t;

static inline int schedee_priority_set(struct schedee *s,
		sched_priority_t new_priority) {
	return 0;
}

static inline sched_priority_t schedee_priority_get(struct schedee *s) {
	return 0;
}

static inline sched_priority_t schedee_priority_inherit(struct schedee *s,
		sched_priority_t priority) {
	return 0;
}

static inline sched_priority_t schedee_priority_reverse(struct schedee *s) {
	return 0;
}


#endif /* SCHED_SIMPLE_PRIORITY_H_ */
