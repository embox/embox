/**
 * @file
 * @brief Fixed time scheduler
 *
 * @date Mar 6, 2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SCHED_STRATEGY_TRIVIAL_H_
#define KERNEL_THREAD_SCHED_STRATEGY_TRIVIAL_H_

#include <lib/list.h>

#include <kernel/thread/sched_priority.h>
#include <kernel/clock_source.h>
#include <time.h>

struct thread;

struct sched_strategy_data {
	struct list_head l_link; /* List data */
};

struct runq {
	struct thread *current;  /* Current thread */
	struct list_head rq;     /* Ready queue */

	useconds_t timeleft;     /* Time left for thread */
	useconds_t last_upd;    /* Last updating of timeleft */
};

struct sleepq {
	struct list_head rq;     /* Resume queue */
	struct list_head sq;     /* Suspend queue */
};

#endif /* KERNEL_THREAD_SCHED_STRATEGY_TRIVIAL_H_ */
