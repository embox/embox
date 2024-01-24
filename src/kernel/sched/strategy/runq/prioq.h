/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_QUEUE_PRIOQ_H_
#define KERNEL_THREAD_QUEUE_PRIOQ_H_

#include <lib/libds/priolist.h>

typedef struct priolist      runq_t;
typedef struct priolist_link runq_item_t;

#define __RUNQ_ITEM_INIT(item) \
	PRIOLIST_LINK_INIT(item)

#endif /* KERNEL_THREAD_QUEUE_PRIOQ_H_ */
