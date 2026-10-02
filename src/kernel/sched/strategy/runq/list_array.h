/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_QUEUE_PRIOQ2_H_
#define KERNEL_THREAD_QUEUE_PRIOQ2_H_

#include <lib/libds/dlist.h>

#include <kernel/sched/schedee_priority.h>

struct runq_queue {
	struct dlist_head list[SCHED_PRIORITY_TOTAL];
};

typedef struct dlist_head runq_item_t;

typedef struct runq_queue runq_t;

#define __RUNQ_ITEM_INIT(item) \
	DLIST_INIT(item)

#endif /* KERNEL_THREAD_QUEUE_PRIOQ2_H_ */
