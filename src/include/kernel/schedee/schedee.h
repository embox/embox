/**
 * @file
 * @brief
 *
 * @date 21.11.2013
 * @author Andrey Kokorev
 */

#ifndef _KERNEL_SCHEDEE_H_
#define _KERNEL_SCHEDEE_H_


#include <kernel/sched/sched_strategy.h>
#include <kernel/thread/signal.h>
#include <kernel/sched/waitq.h>

#define SCHEDEE_EXIT   0
#define SCHEDEE_REPEAT 1

struct schedee {
	/* Process function is called in schedule() function after extracting
	 * next schedee from runq. This function performs all necessary actions
	 * with specific schedee implementation.
	 * It has to restore rq->ipl as soon as possible.
	 * This function also return one of the SCHEDEE_* value in order to
	 * inform scheduler whether it is to finish scheduling (SCHEDEE_EXIT)
	 * or to get another schedee and repeat it (SCHEDEE_REPEAT).
	 */
	int               (*process)(struct schedee *prev, struct schedee *n,
			struct runq *rq);

	void              *(*run)(void *); /**< Start routine */
	void              *run_arg;        /**< Argument to be passed to run */

	struct sched_attr sched_attr;      /**< Scheduler-private data pointer */

	unsigned int      active;       /**< Running on a CPU. TODO SMP-only. */
	unsigned int      ready;        /**< Managed by the scheduler. */
	unsigned int      waiting;      /**< Waiting for an event. */

	spinlock_t        lock;         /**< Protects wait state and others. */

	struct sigstate   sigstate;     /**< Pending signal(s). */
	struct waitq_link waitq_link;
};

#endif /* _KERNEL_SCHEDEE_H_ */
