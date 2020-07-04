/**
 * @file
 *
 * @date Jul 31, 2013
 * @author: Anton Bondarev
 * @author: Anton Kozlov
 */

#ifndef AFFINITY_NONE_H_
#define AFFINITY_NONE_H_

#include <sys/cdefs.h>

struct affinity {
	EMPTY_STRUCT_BODY
};

static inline int sched_affinity_check(struct affinity *a, int mask) {
	return 1;
}

static inline void sched_affinity_init(struct affinity *a) { }

/* TODO none affinity shouldn't have set method */
static inline void sched_affinity_set(struct affinity *a, int mask) { }

#define __SCHED_AFFINITY_INIT() \
	{ }

#endif /* AFFINITY_NONE_H_ */
