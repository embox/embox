/**
 * @file
 * @brief
 *
 * @date 14.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_EVENT_SCHED_H_
#define KERNEL_EVENT_SCHED_H_

#include <kernel/thread/sched.h>
#include <kernel/thread/wait_queue.h>

#define EVENT_TIMEOUT_INFINITE SCHED_TIMEOUT_INFINITE

struct event {
	struct wait_queue wait_queue;
};

extern int __event_wait(unsigned long timeout); /* TODO: Remove this */

#define EVENT_WAIT(event, cond_expr, timeout) \
	((cond_expr) ? 0 : ({ \
		struct wait_link __wait_link; \
		int __wait_ret; \
		do { \
			wait_queue_prepare(&__wait_link); \
			if (cond_expr) { \
				__wait_ret = 0; \
			} else { \
				wait_queue_insert(&(event)->wait_queue, &__wait_link); \
				__wait_ret = __event_wait(timeout); \
			} \
			wait_queue_cleanup(&__wait_link); \
			if (__wait_ret == -EINTR) \
				continue; \
		} while(0); \
		\
		__wait_ret; \
	}))

#endif /* KERNEL_EVENT_SCHED_H_ */

