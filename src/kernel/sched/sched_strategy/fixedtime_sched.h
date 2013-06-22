/**
 * @file
 * @brief Fixed time scheduler
 *
 * @date Mar 6, 2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SCHED_STRATEGY_FIXEDTIME_H_
#define KERNEL_THREAD_SCHED_STRATEGY_FIXEDTIME_H_

#include <util/dlist.h>

#include <kernel/time/timer.h>
#include <kernel/sched/startq.h>
#include <kernel/sched/sched_priority.h>
#include <kernel/task.h>

struct thread;

struct sched_strategy_data {
	struct dlist_head l_link;
};

struct runq {
	struct thread *current;
	struct dlist_head rq;

	sys_timer_t *tick_timer;
};

struct sleepq {
	struct dlist_head sq;

	struct startq_data startq_data;
};

#endif /* KERNEL_THREAD_SCHED_STRATEGY_FIXEDTIME_H_ */
