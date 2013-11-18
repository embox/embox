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

#define EVENT_TIMEOUT_INFINITE SCHED_TIMEOUT_INFINITE

struct event {
	struct waitq waitq;
};
__BEGIN_DECLS

/* TODO: Remove this */
extern int __event_wait(struct event *event, unsigned long timeout);

__END_DECLS

#define __EVENT_WAIT(event, cond_expr, timeout, intr)         \
	((cond_expr) ? 0 : ({                                     \
		int __wait_ret;                                       \
		struct wait_link wait_link;                           \
		do {                                                  \
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
		} while (0);                                          \
		__wait_ret;                                           \
	}))

#define EVENT_WAIT(event, cond_expr, timeout) \
	__EVENT_WAIT(event, cond_expr, timeout, 0)

#define EVENT_WAIT_OR_INTR(event, cond_expr, timeout) \
	__EVENT_WAIT(event, cond_expr, timeout, 1)

#endif /* KERNEL_EVENT_EVENT_SCHED_H_ */
