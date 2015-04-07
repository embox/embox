/**
 * @file
 * @brief Defines thread-specific methods associated with waitq. You can also
 * or instead use ones from schedee/waitq.h
 *
 * @date 7.08.2014
 * @author Vita Loginova
 */

#ifndef KERNEL_THREAD_WAITQ_H_
#define KERNEL_THREAD_WAITQ_H_

#include <kernel/sched.h>
#include <kernel/sched/waitq.h>
#include <kernel/sched/current.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/thread/signal_lock.h>
#include <kernel/time/time.h>

#define WAITQ_WAIT_TIMEOUT(wq, cond_expr, timeout) \
	((cond_expr) ? 0 : ({                                            \
		struct waitq_link *wql = &schedee_get_current()->waitq_link; \
		clock_t __wait_timeout = timeout == SCHED_TIMEOUT_INFINITE ? \
			SCHED_TIMEOUT_INFINITE : ms2jiffies(timeout);            \
		int __wait_ret = 0;                                          \
		waitq_link_init(wql);                                       \
		                                                             \
		threadsig_lock();                                            \
		do {                                                         \
			waitq_wait_prepare(wq, wql);                            \
			                                                         \
			if (cond_expr)                                           \
				break;                                               \
			                                                         \
			__wait_ret = sched_wait_timeout(__wait_timeout,          \
				                            &__wait_timeout);        \
		} while (!__wait_ret);                                       \
		                                                             \
		waitq_wait_cleanup(wq, wql);                                \
		threadsig_unlock();                                          \
		                                                             \
		__wait_ret;                                                  \
	}))

#define WAITQ_WAIT_ONCE(wq, timeout) \
	({                                                               \
	 	struct waitq_link *wql = &schedee_get_current()->waitq_link; \
		clock_t __wait_timeout = timeout == SCHED_TIMEOUT_INFINITE ? \
			SCHED_TIMEOUT_INFINITE : ms2jiffies(timeout);            \
		int __wait_ret = 0;                                          \
		                                                             \
		waitq_link_init(wql);                                       \
		                                                             \
		threadsig_lock();                                            \
		waitq_wait_prepare(wq, wql);                                \
		sched_wait_timeout(__wait_timeout, NULL);                    \
		waitq_wait_cleanup(wq, wql);                                \
		                                                             \
		threadsig_unlock();                                          \
		__wait_ret;                                                  \
	})

#define WAITQ_WAIT(wq, cond_expr) \
	WAITQ_WAIT_TIMEOUT(wq, cond_expr, SCHED_TIMEOUT_INFINITE)

#endif /* KERNEL_THREAD_WAITQ_H_ */
