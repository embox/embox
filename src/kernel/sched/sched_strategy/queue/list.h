/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_QUEUE_LIST_H_
#define KERNEL_THREAD_QUEUE_LIST_H_

#include <util/dlist.h>

struct sched_strategy_data {
	struct dlist_head l_link;
};

typedef struct dlist_head runq_queue_t;
typedef struct dlist_head sleepq_queue_t;

#endif /* KERNEL_THREAD_QUEUE_LIST_H_ */
