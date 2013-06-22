/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_QUEUE_PRIOQ_H_
#define KERNEL_THREAD_QUEUE_PRIOQ_H_

#include <util/prioq.h>

struct sched_strategy_data {
	struct prioq_link pq_link;
};

typedef struct prioq runq_queue_t;
typedef struct prioq sleepq_queue_t;

#endif /* KERNEL_THREAD_QUEUE_PRIOQ_H_ */
