/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_WAIT_QUEUE_H_
#define KERNEL_THREAD_WAIT_QUEUE_H_

#include <util/dlist.h>

#include <kernel/work.h>


struct wait_queue {
	struct dlist_head list;

	int flag;
};

static inline void wait_queue_init(struct wait_queue *wait_queue) {
	dlist_init(&wait_queue->list);
}

#endif /* KERNEL_THREAD_WAIT_QUEUE_H_ */

