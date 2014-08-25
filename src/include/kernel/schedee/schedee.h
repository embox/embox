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

#include <kernel/sched/runq.h>
#include <kernel/sched/affinity.h>
#include <kernel/sched/runq.h>
#include <kernel/sched/sched_timing.h>
#include <kernel/sched/sched_priority.h>
#include <kernel/schedee/schedee_priority.h>

#include <kernel/sched/waitq.h>
/* Used as a return value of schedee->process function. */
/* Informs the scheduler that it has to finish scheduling. */
#define SCHEDEE_EXIT   0
/* Informs the scheduler that it has to get another schedee and repeat
 * scheduling. */
#define SCHEDEE_REPEAT 1

struct schedee {

	runq_item_t       runq_link;

	spinlock_t        lock;         /**< Protects wait state and others. */

	/* Process function is called in schedule() function after extracting
	 * the next schedee from the runq. This function performs all necessary
	 * actions with a specific schedee implementation.
	 *
	 * It has to restore ipl as soon as possible.
	 *
	 * This function returns one of the SCHEDEE_* value.
	 */
	int               (*process)(struct schedee *prev, struct schedee *next,
			ipl_t ipl);

	void              *(*run)(void *); /**< Start routine */
	void              *run_arg;        /**< Argument to be passed to run */
	unsigned int      active;       /**< Running on a CPU. TODO SMP-only. */
	unsigned int      ready;        /**< Managed by the scheduler. */
	unsigned int      waiting;      /**< Waiting for an event. */

	affinity_t        affinity;
	sched_timing_t    sched_time;
	schedee_priority_t thread_priority;
	int               policy;

	struct waitq_link waitq_link;   /**< Used as a link in different waitqs. */
};

static inline int schedee_init(struct schedee *schde) {
	runq_item_init(&schde->runq_link);
	sched_affinity_init(schde);
	sched_timing_init(schde);
	return 0;
}

extern int schedee_init(struct schedee *schdee);


#endif /* _KERNEL_SCHEDEE_H_ */
