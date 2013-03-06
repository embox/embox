/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_QUEUE_PRIOQ_H_
#define KERNEL_THREAD_QUEUE_PRIOQ_H_

#include <util/dlist.h>

#include <kernel/thread/sched_priority.h>

struct prior_queue {
	struct dlist_head list[THREAD_PRIORITY_TOTAL];
//	int mask[MASK_LEN];
};

struct sched_strategy_data {
	struct dlist_head link;
};

typedef struct prior_queue runq_queue_t;
typedef struct prior_queue sleepq_queue_t;

#endif /* KERNEL_THREAD_QUEUE_PRIOQ_H_ */
