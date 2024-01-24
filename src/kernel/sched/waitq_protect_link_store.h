/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.05.2014
 */

#ifndef SCHED_WAITQ_PROTECT_THREAD_H_
#define SCHED_WAITQ_PROTECT_THREAD_H_

#include <lib/libds/dlist.h>

struct waitq_protect_link {
	struct dlist_head waitq_protect_list;
};

#endif /* SCHED_WAITQ_PROTECT_THREAD_H_ */


