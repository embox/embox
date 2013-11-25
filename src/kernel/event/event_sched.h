/**
 * @file
 * @brief
 *
 * @date 14.05.13
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#ifndef KERNEL_EVENT_EVENT_SCHED_H_
#define KERNEL_EVENT_EVENT_SCHED_H_

#include <kernel/sched.h>
#include <kernel/sched/waitq.h>

__BEGIN_DECLS

#define EVENT_TIMEOUT_INFINITE \
	SCHED_TIMEOUT_INFINITE

struct event {
	struct waitq waitq;
};

#define __EVENT_WAIT(event, cond_expr, timeout, intr)     \
	((cond_expr) ? 0 : ({                                 \
		int __wait_ret;                                   \
		struct wait_link wait_link;                       \
		while(1) {                                        \
			__waitq_prepare(&(event)->waitq, &wait_link); \
			if (cond_expr) {                              \
				__wait_ret = 0;                           \
				break;                                    \
			}                                             \
			__wait_ret = __event_wait(event, timeout);    \
			if (__wait_ret == -ETIMEDOUT) {               \
				break;                                    \
			}                                             \
			if (!intr && (__wait_ret == -EINTR)) {        \
				break;                                    \
			}                                             \
		}                                                 \
		if (cond_expr) {                                  \
			__wait_ret = 0;                               \
		}                                                 \
		__waitq_cleanup();                                \
		__wait_ret;                                       \
	}))

/* TODO: Remove this */
extern int __event_wait(struct event *event, unsigned long timeout);

__END_DECLS

#endif /* KERNEL_EVENT_EVENT_SCHED_H_ */
