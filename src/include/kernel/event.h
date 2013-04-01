/**
 * @file
 * @brief Waiting on events and async notifying (scheduler-unaware) API.
 *
 * @date 28.03.13
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_EVENT_H_
#define KERNEL_EVENT_H_

#include <kernel/work.h>
#include <util/dlist.h>

struct event {
	struct work_queue work_queue;
};

struct __event_wait {
	struct work work;
};

#define EVENT_WAIT(e, cond_expr) \
	__EVENT_WAIT(e, cond_expr, -1, 1)

#define EVENT_WAIT_TIMEOUT(e, cond_expr, timeout) \
	__EVENT_WAIT(e, cond_expr, timeout, 1)

#define EVENT_WAIT_UNINTERRUPTIBLE(e, cond_expr) \
	__EVENT_WAIT(e, cond_expr, -1, 0)

#define EVENT_WAIT_UNINTERRUPTIBLE_TIMEOUT(e, cond_expr, timeout) \
	__EVENT_WAIT(e, cond_expr, timeout, 0)

#define __EVENT_WAIT(e, cond_expr, timeout, interruptible) \
	((cond_expr) ? 0 : ({                                                     \
		struct __event_wait __wait;                                           \
		int __time_left = timeout;                                            \
		int __wait_ret = 0;                                                   \
                                                                              \
		__event_wait_init(&__wait);                                           \
                                                                              \
		do {                                                                  \
			__event_prepare_wait(e, &__wait);                                 \
                                                                              \
			if (cond_expr)                                                    \
				__wait_ret = 0;                                               \
                                                                              \
			else if (!__wait_ret) {                                           \
				/* Zzz... */                                                  \
				__time_left = __event_do_wait(e, __time_left);                \
                                                                              \
				if (!__time_left)                                             \
					__wait_ret = -ETIMEDOUT;                                  \
				else if (interruptible)                                       \
					__wait_ret = -EINTR;                                      \
                                                                              \
				continue;                                                     \
			}                                                                 \
		} while(0);                                                           \
                                                                              \
		__event_cleanup_wait(e, &__wait);                                     \
                                                                              \
		__wait_ret;                                                           \
	}))

static inline struct event *event_init(struct event *e) {
	work_queue_init(&e->work_queue);
	return e;
}

static inline struct __event_wait *__event_wait_init(struct __event_wait *ew) {
	extern int __event_wait_waker(struct work *);
	work_init(&ew->work, __event_wait_waker, 0);
	return ew;
}

extern void event_wake(struct event *);

extern void __event_prepare_wait(struct event *, struct __event_wait *);
extern long __event_do_wait(struct event *, long timeout);
extern void __event_cleanup_wait(struct event *, struct __event_wait *);

#endif /* KERNEL_EVENT_H_ */
