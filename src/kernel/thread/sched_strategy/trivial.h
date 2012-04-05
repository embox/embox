/**
 * @file
 * @brief Trivial scheduler based on lists
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SCHED_STRATEGY_TRIVIAL_H_
#define KERNEL_THREAD_SCHED_STRATEGY_TRIVIAL_H_

#include <util/dlist.h>
#include <kernel/thread/sched_priority.h>

struct thread;

struct sched_strategy_data {
	struct dlist_head l_link; /* List data */
};

struct runq {
	struct thread *current;  /* Current thread */
	struct dlist_head rq;     /* Ready queue */
};

struct sleepq {
	struct dlist_head rq;     /* Resume queue */
	struct dlist_head sq;     /* Suspend queue */
};

#endif /* KERNEL_THREAD_SCHED_STRATEGY_TRIVIAL_H_ */
