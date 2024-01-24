/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_QUEUE_LIST_H_
#define KERNEL_THREAD_QUEUE_LIST_H_

#include <lib/libds/dlist.h>

typedef struct dlist_head runq_item_t;

typedef struct dlist_head runq_t;

#define __RUNQ_ITEM_INIT(item) \
	DLIST_INIT(item)

#endif /* KERNEL_THREAD_QUEUE_LIST_H_ */
