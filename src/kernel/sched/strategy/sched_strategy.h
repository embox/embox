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
#include <kernel/schedee/schedee_priority.h>

struct runq {
	runq_t     queue;
	spinlock_t lock;
};

#endif /* KERNEL_THREAD_SCHED_STRATEGY_H_ */
