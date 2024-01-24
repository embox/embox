/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.05.2014
 */

#ifndef THREAD_WAIT_H_
#define THREAD_WAIT_H_

#include <lib/libds/dlist.h>

struct thread_wait {
	struct dlist_head thread_waitq_list;
};

extern void thread_wait_init(struct thread_wait *tw);

extern void thread_wait_deinit(struct thread_wait *tw);

#endif /* THREAD_WAIT_H_ */

