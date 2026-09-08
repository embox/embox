/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_QUEUE_PRIOQ2_H_
#define KERNEL_THREAD_QUEUE_PRIOQ2_H_

#include <stdint.h>

#include <lib/libds/dlist.h>

#include <kernel/sched/schedee_priority.h>

/* How many 64-bit words cover the priority range. */
#define RUNQ_MAP_WORDS ((SCHED_PRIORITY_TOTAL + 63) / 64)

struct runq_queue {
	struct dlist_head list[SCHED_PRIORITY_TOTAL];
	/**
	 * One bit per priority: set while that level's list is non-empty.
	 *
	 * Without it every search is a walk over all 256 levels, and the
	 * scheduler does three or four of them per switch with the BKL held.
	 */
	uint64_t occupied[RUNQ_MAP_WORDS];
};

typedef struct dlist_head runq_item_t;

typedef struct runq_queue runq_t;

#define __RUNQ_ITEM_INIT(item) \
	DLIST_INIT(item)

#endif /* KERNEL_THREAD_QUEUE_PRIOQ2_H_ */
