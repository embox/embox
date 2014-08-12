/**
 * @file
 * @brief Defines schedee structure. Schedee is an abstract plannable entity
 *        operated by the scheduler.
 *
 * @date 21.11.2013
 * @author Andrey Kokorev
 * @author Vita Loginova
 */

#ifndef _KERNEL_SCHEDEE_H_
#define _KERNEL_SCHEDEE_H_


#include <kernel/sched/sched_strategy.h>
#include <kernel/thread/signal.h>
#include <kernel/sched/waitq.h>

/* Used as a return value of schedee->process function. */
/* Informs the scheduler that it has to finish scheduling. */
#define SCHEDEE_EXIT   0
/* Informs the scheduler that it has to get another schedee and repeat
 * scheduling. */
#define SCHEDEE_REPEAT 1

struct schedee {
	/* Process function is called in schedule() function after extracting
	 * the next schedee from the runq. This function performs all necessary
	 * actions with a specific schedee implementation.
	 *
	 * It has to restore rq->ipl as soon as possible.
	 *
	 * This function returns one of the SCHEDEE_* value.
	 */
	int               (*process)(struct schedee *prev, struct schedee *next,
			struct runq *rq);

	void              *(*run)(void *); /**< Start routine */
	void              *run_arg;        /**< Argument to be passed to run */

	struct sched_attr sched_attr;      /**< Scheduler-private data pointer */

	unsigned int      active;       /**< Running on a CPU. TODO SMP-only. */
	unsigned int      ready;        /**< Managed by the scheduler. */
	unsigned int      waiting;      /**< Waiting for an event. */

	spinlock_t        lock;         /**< Protects wait state and others. */

	struct waitq_link waitq_link;   /**< Used as a link in different waitqs. */
};

#endif /* _KERNEL_SCHEDEE_H_ */
