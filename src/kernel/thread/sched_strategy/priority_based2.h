/**
 * @file
 * @brief
 *
 * @date 05.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SCHED_STRATEGY_PRIORITY_BASED_H_
#define KERNEL_THREAD_SCHED_STRATEGY_PRIORITY_BASED_H_

#include <util/dlist.h>

#include <kernel/thread/sched_priority.h>
#include <kernel/thread/startq.h>

struct thread;
struct sys_timer;

struct sched_strategy_data {
	struct dlist_head link;
};

struct prior_queue {
	struct dlist_head list[THREAD_PRIORITY_TOTAL];
//	int mask[MASK_LEN];
};

struct runq {
	struct prior_queue queue;

	struct sys_timer *tick_timer;
};

struct sleepq {
	struct prior_queue queue;

	struct startq_data startq_data;
};



#endif /* KERNEL_THREAD_SCHED_STRATEGY_PRIORITY_BASED_H_ */
