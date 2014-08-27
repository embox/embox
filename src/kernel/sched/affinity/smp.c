/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 */

#include <kernel/sched/affinity.h>

/** Default schedee affinity mask */
#define SCHEDEE_AFFINITY_NONE         ((unsigned int)-1)

int sched_affinity_check(struct affinity *a, int mask) {
	return !!(a->mask & mask);
}

void sched_affinity_init(struct affinity *a) {
	a->mask = SCHEDEE_AFFINITY_NONE;
}

void sched_affinity_set(struct affinity *a, int mask) {
	a->mask = mask;
}

int sched_affinity_get(struct affinity *a) {
	return a->mask;
}
