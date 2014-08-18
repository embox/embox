/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#ifndef NO_AFFINITY_H_
#define NO_AFFINITY_H_

struct schedee;

typedef struct { } __affinity_t;

static inline int sched_affinity_check(struct schedee *s, int mask) {
	(void)s;
	(void)mask;
	return 1;
}

static inline void sched_affinity_init(struct schedee *s) {
	(void)s;
	return;
}

static inline void sched_affinity_set(struct schedee *s, int mask) {
	(void)s;
	(void)mask;
	return;
}

static inline int sched_affinity_get(struct schedee *s) {
	(void)s;
	return 0;
}

#endif /* NO_AFFINITY_H_ */
