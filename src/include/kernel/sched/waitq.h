/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_SCHED_WAITQ_H_
#define KERNEL_SCHED_WAITQ_H_

#include <assert.h>

#include <sys/cdefs.h>
#include <util/dlist.h>
#include <kernel/spinlock.h>
#include <kernel/sched.h>
#include <kernel/thread/current.h>
#include <kernel/thread/signal_lock.h>

#include <kernel/time/time.h>

__BEGIN_DECLS

struct thread;

struct waitq {
	struct dlist_head list;
	spinlock_t lock;
};

struct waitq_link {
	struct dlist_head link;
	struct thread *thread;
};

#define WAITQ_INIT(wq)  { \
	.list = DLIST_INIT(wq.list),  \
	.lock = SPIN_STATIC_UNLOCKED, \
}

static inline void waitq_init(struct waitq *wq) {
	dlist_init(&wq->list);
	wq->lock = SPIN_UNLOCKED;
}

static inline void waitq_link_init(struct waitq_link *wql) {
	//wql->thread = thread_self();
	wql->thread = thread_get_current();
	dlist_head_init(&wql->link);
}

extern void __waitq_add(struct waitq *, struct waitq_link *);
extern void waitq_add(struct waitq *, struct waitq_link *);

extern void __waitq_del(struct waitq *, struct waitq_link *);
extern void waitq_del(struct waitq *, struct waitq_link *);

extern void waitq_wait_prepare(struct waitq *, struct waitq_link *);
extern void waitq_wait_cleanup(struct waitq *, struct waitq_link *);

extern void __waitq_wakeup(struct waitq *, int nr);
extern void waitq_wakeup(struct waitq *, int nr);
static inline void waitq_wakeup_all(struct waitq *wq) {
	waitq_wakeup(wq, 0);
}

#define WAITQ_WAIT_TIMEOUT(wq, cond_expr, timeout) \
	((cond_expr) ? 0 : ({                                            \
		struct waitq_link wql;                                       \
		clock_t __wait_timeout = timeout == SCHED_TIMEOUT_INFINITE ? \
			SCHED_TIMEOUT_INFINITE : ms2jiffies(timeout);            \
		int __wait_ret = 0;                                          \
		waitq_link_init(&wql);                                       \
		                                                             \
		threadsig_lock();                                            \
		do {                                                         \
			waitq_wait_prepare(wq, &wql);                            \
			                                                         \
			if (cond_expr)                                           \
				break;                                               \
			                                                         \
			__wait_ret = sched_wait_timeout(__wait_timeout,          \
				                            &__wait_timeout);        \
		} while (!__wait_ret);                                       \
		                                                             \
		waitq_wait_cleanup(wq, &wql);                                \
		threadsig_unlock();                                          \
		                                                             \
		__wait_ret;                                                  \
	}))

#define WAITQ_WAIT_ONCE(wq, timeout) \
	({                                                               \
	 	struct waitq_link wql;                                       \
		clock_t __wait_timeout = timeout == SCHED_TIMEOUT_INFINITE ? \
			SCHED_TIMEOUT_INFINITE : ms2jiffies(timeout);            \
		int __wait_ret = 0;                                          \
		                                                             \
		waitq_link_init(&wql);                                       \
		                                                             \
		threadsig_lock();                                            \
		waitq_wait_prepare(wq, &wql);                                \
		sched_wait_timeout(__wait_timeout, NULL);                    \
		waitq_wait_cleanup(wq, &wql);                                \
		                                                             \
		threadsig_unlock();                                          \
		__wait_ret;                                                  \
	})

#define WAITQ_WAIT(wq, cond_expr) \
	WAITQ_WAIT_TIMEOUT(wq, cond_expr, SCHED_TIMEOUT_INFINITE)

__END_DECLS

#endif /* KERNEL_SCHED_WAITQ_H_ */
