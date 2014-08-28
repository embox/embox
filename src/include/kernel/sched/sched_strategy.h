/**
 * @file
 * @brief Defines the simplest operations in scheduler. Implementation depends
 * on the scheduling algorithm. Part of method is used in other functions.
 * All of policy must implement its interface.
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 * @author Bulychev Anton
 *          - Description of functions
 */

#ifndef KERNEL_SCHED_SCHED_STRATEGY_H_
#define KERNEL_SCHED_SCHED_STRATEGY_H_

#include <kernel/sched/runq.h>
#include <kernel/spinlock.h>

struct runq {
	runq_t queue;
	spinlock_t lock;
};

#endif /* KERNEL_SCHED_SCHED_STRATEGY_H_ */
