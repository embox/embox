/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#ifndef NO_AFFINITY_H_
#define NO_AFFINITY_H_

struct thread;

typedef struct { } __affinity_t;

static inline int sched_affinity_check(struct thread *t, int mask) {
	return 1;
}

static inline void sched_affinity_init(struct thread *t) {
	return;
}

static inline void sched_affinity_set(struct thread *t, int mask) {
	return;
}

static inline int sched_affinity_get(struct thread *t) {
	return 0;
}

#endif /* NO_AFFINITY_H_ */
