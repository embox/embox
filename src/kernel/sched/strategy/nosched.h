/**
 * @file
 *
 * @date Mar 21, 2013
 * @author: Anton Bondarev
 */

#ifndef SCHED_NO_STRATEGY_H_
#define SCHED_NO_STRATEGY_H_

#include <kernel/sched/runq.h>
#include <kernel/spinlock.h>

struct runq {
	runq_t queue;
	spinlock_t lock;
};

struct sched_attr {
	int policy;
};

#endif /* SCHED_NO_STRATEGY_H_ */
