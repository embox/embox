/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#ifndef SMP_AFFINITY_H_
#define SMP_AFFINITY_H_

struct affinity {
	int mask;
};

/** Default schedee affinity mask */
#define SCHEDEE_AFFINITY_NONE         ((unsigned int)-1)

/* These are field reads, and runq_get_next() does one per schedee per
 * schedule; out of line, at -O0, that is a measurable share of the kernel's
 * time. sched_affinity_init() stays in the .c file: it reads a module option. */
#define __SCHED_AFFINITY_INLINED

static inline int sched_affinity_check(struct affinity *a, int mask) {
	return !!(a->mask & mask);
}

static inline void sched_affinity_set(struct affinity *a, int mask) {
	a->mask = mask;
}

static inline int sched_affinity_get(struct affinity *a) {
	return a->mask;
}

#define __SCHED_AFFINITY_INIT() \
	{ .mask = SCHEDEE_AFFINITY_NONE }

#endif /* SMP_AFFINITY_H_ */
