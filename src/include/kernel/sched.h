/**
 * @file
 * @brief TODO -- Alina.
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 * @author Kirill Skorodumov
 * @author Alina Kramar
 */

#ifndef KERNEL_SCHED_H_
#define KERNEL_SCHED_H_

#include <sys/types.h>

#include <hal/cpu.h>

#include <kernel/sched/sched_lock.h>
#include <kernel/sched/sched_priority.h>

#include <kernel/time/time.h>

#define SCHED_TIMEOUT_INFINITE     (unsigned long)(-1)

struct lthread;
struct thread;

__BEGIN_DECLS

/**
 * Initializes scheduler.
 *
 * @param current
 *   Thread structure corresponding to the current control flow.
 * @param idle
 *   Thread that should be executed in the last resort.
 * @return
 *   Operation result.
 * @retval 0
 *   If initialization succeeds.
 * @retval non-zero
 *   Error code if initialization of the underlying policy implementation has
 *   failed.
 */
extern int sched_init(struct thread *current, struct thread *idle);

extern void sched_ticker_init(void);
extern void sched_ticker_fini(void);
extern void sched_ticker_switch(struct thread *prev, struct thread *next);

extern void sched_sleep(void);

/**
 * Makes active or waking thread and adds thread to the queue of ready to
 * executing threads.
 *
 * @param thread
 *   Thread which will be added.
 */
extern void __sched_start(struct thread *t);
extern void sched_start(struct thread *t);

/**
 * Adds lthread to runq.
 * @param lwt
 *   Lwthread which will be added.
 */
extern void sched_wakeup_l(struct lthread *lt);

#if 0
/**
 * Makes exit thread and removes thread from scheduler.
 */
extern void sched_finish(struct thread *t);
#endif

extern int sched_active(struct thread *t);

/**
 * Changes scheduling priority of the thread. If the thread is running now
 * (present in the runq) calls runq_change_priority() for rescheduling runq
 * and thread_set_priority() else just calls thread_set_priority() for setup
 * a new value for priority field in sched_strategy_data structure
 *
 * @param t
 *   Thread to operate with.
 * @param priority
 *   New scheduling priority of the the thread.
 */
extern int sched_change_priority(struct thread *t, sched_priority_t priority);

extern void sched_wait_prepare(void);
extern void sched_wait_cleanup(void);

extern int sched_wait(void);
extern int sched_wait_timeout(clock_t timeout, clock_t *remain);

extern int __sched_wakeup(struct thread *);
extern int sched_wakeup(struct thread *);

/* XXX thread will not be ever in runq or active - thread is dead,
 * but with allocated resources on its stack */
extern void sched_freeze(struct thread *t);

/**
 * Wait cond_expr to become TRUE.
 */
#define SCHED_WAIT(cond_expr) \
	SCHED_WAIT_TIMEOUT(cond_expr, SCHED_TIMEOUT_INFINITE)

/**
 * Requests switching of the current thread.
 */
extern void sched_post_switch(void);

/**
 * Runs the scheduler right now.
 */
extern void schedule(void);

extern void sched_thread_switch(struct thread *prev, struct thread *next);

extern void sched_ack_switched(void);

/**
 * @brief Makes thread to run regardless of it's state if thread is scheduling
 *
 * @param thread Thread to operate with
 *
 * @return
 *   Operation result
 * @retval 0
 *   On success.
 * @retval non-zero
 *   On operation fail.
 */
extern void sched_signal(struct thread *thread);

__END_DECLS

#include <kernel/thread/signal_lock.h>

#define SCHED_WAIT_TIMEOUT(cond_expr, timeout) \
	((cond_expr) ? 0 : ({                                            \
		int __wait_ret = 0;                                          \
		clock_t __wait_timeout = timeout == SCHED_TIMEOUT_INFINITE ? \
			SCHED_TIMEOUT_INFINITE : ms2jiffies(timeout);            \
		                                                             \
		threadsig_lock();                                            \
		do {                                                         \
			sched_wait_prepare();                                    \
			                                                         \
			if (cond_expr)                                           \
				break;                                               \
			                                                         \
			__wait_ret = sched_wait_timeout(__wait_timeout,          \
											&__wait_timeout);        \
		} while (!__wait_ret);                                       \
		                                                             \
		sched_wait_cleanup();                                        \
		                                                             \
		threadsig_unlock();                                          \
		__wait_ret;                                                  \
	}))


#endif /* KERNEL_SCHED_H_ */
