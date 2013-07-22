/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_QUEUE_PRIOQ2_H_
#define KERNEL_THREAD_QUEUE_PRIOQ2_H_

#include <util/dlist.h>
#include <util/prioq.h>

#include <kernel/sched/sched_priority.h>

struct runq_queue {
	struct dlist_head list[SCHED_PRIORITY_TOTAL];

/* TODO: */
//	int mask[MASK_LEN];
};

struct sched_strategy_data {
	struct dlist_head link;     /* If thread is in runq */
	struct prioq_link pq_link;  /* If thread is in sleepq */
};

typedef struct runq_queue runq_queue_t;

#endif /* KERNEL_THREAD_QUEUE_PRIOQ2_H_ */
