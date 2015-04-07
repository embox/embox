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
#include <kernel/sched/sched_timing.h>
#include <kernel/sched/schedee_priority.h>

#include <kernel/sched/waitq.h>

#ifdef SMP
# define TW_SMP_WAKING  (~0x0)  /**< In the middle of sched_wakeup. */
#else
# define TW_SMP_WAKING  (0x0)   /* Not used in non-SMP kernel. */
#endif

struct schedee {

	runq_item_t       runq_link;

	spinlock_t        lock; /**< Protects wait state and others. */

	/* Process function is called in schedule() function after extracting
	 * the next schedee from the runq. This function performs all necessary
	 * actions with a specific schedee implementation.
	 *
	 * It has to restore ipl as soon as possible.
	 *
	 * This function returns schedee to which context switched.
	 */
	struct schedee    *(*process)(struct schedee *prev, struct schedee *next);

	unsigned int      active;          /**< Running on a CPU. TODO SMP-only. */
	unsigned int      ready;           /**< Managed by the scheduler. */
	unsigned int      waiting;         /**< Waiting for an event. */

	struct affinity         affinity;
	struct sched_timing     sched_timing;
	struct schedee_priority priority;

	struct waitq_link waitq_link; /**< Used as a link in different waitqs. */
};

#include <stdbool.h>
static inline int schedee_init(struct schedee *schedee, int priority,
	struct schedee *(*process)(struct schedee *prev, struct schedee *next))
{
	runq_item_init(&schedee->runq_link);

	schedee->lock = SPIN_UNLOCKED;

	schedee->process = process;

	schedee->ready = false;
	schedee->active = false;
	schedee->waiting = true;

	schedee_priority_init(schedee, priority);
	sched_affinity_init(&schedee->affinity);
	sched_timing_init(schedee);

	return 0;
}

extern int schedee_init(struct schedee *schedee, int priority,
	struct schedee *(*process)(struct schedee *prev, struct schedee *next));


#endif /* _KERNEL_SCHEDEE_H_ */
