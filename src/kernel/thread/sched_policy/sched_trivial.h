/**
 * @file
 * @brief Trivial scheduler based on lists
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SCHED_POLICY_TRIVIAL_H_
#define KERNEL_THREAD_SCHED_POLICY_TRIVIAL_H_

#include <lib/list.h>

#include <kernel/thread/sched_priority.h>

struct thread;

struct sched_strategy_data {
	struct list_head l_link; /* List data */
};

struct runq {
	struct thread *current;  /* Current thread */
	struct list_head rq;     /* Ready queue */
};

struct sleepq {
	struct list_head rq;     /* Resume queue */
	struct list_head sq;     /* Suspend queue */
};

#endif /* KERNEL_THREAD_SCHED_POLICY_TRIVIAL_H_ */
