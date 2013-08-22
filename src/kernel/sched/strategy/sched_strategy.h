/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SCHED_STRATEGY_H_
#define KERNEL_THREAD_SCHED_STRATEGY_H_

#include <kernel/sched/affinity.h>
#include <kernel/sched/runq.h>
#include <kernel/sched/sched_timing.h>
#include <kernel/sched/sched_priority.h>

struct runq {
	runq_queue_t queue;
};

struct sched_attr {
	runq_item_t runq_link;
	affinity_t affinity;
	sched_timing_t sched_time;
	thread_priority_t thread_priority;
};

#endif /* KERNEL_THREAD_SCHED_STRATEGY_H_ */
