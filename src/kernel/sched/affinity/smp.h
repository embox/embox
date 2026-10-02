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

#define __SCHED_AFFINITY_INIT() \
	{ .mask = SCHEDEE_AFFINITY_NONE }

#endif /* SMP_AFFINITY_H_ */
