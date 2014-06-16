/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.05.2014
 */

#ifndef SCHED_WAITQ_PROTECT_THREAD_H_
#define SCHED_WAITQ_PROTECT_THREAD_H_

#include <util/dlist.h>

struct thread_wait {
	struct dlist_head wql_protected_list;
};

#endif /* SCHED_WAITQ_PROTECT_THREAD_H_ */


