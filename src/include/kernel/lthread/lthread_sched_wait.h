/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    12.08.2014
 */

#ifndef KERNEL_LTHREAD_SCHED_WAIT_H_
#define KERNEL_LTHREAD_SCHED_WAIT_H_

#include <kernel/time/timer.h>
#include <kernel/time/time.h>

/* Values for sched_wait_info->status */
#define SCHED_WAIT_FINISHED 0
#define SCHED_WAIT_STARTED 1

struct lthread;

struct sched_wait_info {
	struct sys_timer *tmr;
	clock_t remain;
	clock_t cur_time;
	clock_t prev_time;
	int status;
};

/**
 * Fills @p info with initial values.
 * @param info Auxiliary structure for waiting with timeout to fill.
 */
extern void sched_wait_info_init(struct sched_wait_info *info);

/**
 * Prepares @p self for waiting with @p timeout. Has to be called before
 * #sched_wait_timeout_lthread().
 *
 * @param self    The light thread to prepare for waiting.
 * @param timeout The timeout to wait with.
 */
extern void sched_wait_prepare_lthread(struct lthread *self, clock_t timeout);

/**
 * Reatores @p self state after waiting.
 * @param self The light thread finished waiting.
 */
extern void sched_wait_cleanup_lthread(struct lthread *self);

/**
 * Waits until timeout is exceeded. Timeout is set in
 * #sched_wait_prepare_lthread().
 *
 * @param self The light thread to start waiting.
 *
 * @return Waiting result.
 * @retval 0
 *   Waiting has finished before timeout is exceeded.
 * @retval -EAGAIN
 *   Waiting started. The light thread has to return from its routine in
 *   order to be waken up by some schedee or when timeout is exceeded.
 * @retval -ETIMEDOUT
 *   Waiting has finished because of exceeded timeout.
 */
extern int sched_wait_timeout_lthread(struct lthread *self);

/**
 * Waits with timeout till @p cond_expr becomes true.
 *
 * @param self
 *   The light thread to start waiting.
 * @param cond_expr
 *   The conditional expression which is expected to become true.
 * @param timeout
 *   The timeout to wait with.
 *
 * @return Waiting result.
 * @retval 0
 *   Waiting has finished before timeout is exceeded. @p cond_expr is true.
 * @retval -EAGAIN
 *   Waiting started. The light thread has to return from its routine in
 *   order to be waken up by some schedee or when timeout is exceeded.
 * @retval -ETIMEDOUT
 *   Waiting has finished because of exceeded timeout.
 */
#define SCHED_WAIT_TIMEOUT_LTHREAD(self, cond_expr, timeout) \
	({                                                               \
		int __wait_ret = 0;                                          \
		clock_t __wait_timeout = timeout == SCHED_TIMEOUT_INFINITE ? \
			SCHED_TIMEOUT_INFINITE : ms2jiffies(timeout);            \
		                                                             \
		if (!(cond_expr)) {                                          \
			sched_wait_prepare_lthread(self, __wait_timeout);        \
			                                                         \
			__wait_ret = sched_wait_timeout_lthread(self);           \
			                                                         \
			if (__wait_ret == 0) {                                   \
			__wait_ret = sched_wait_timeout_lthread(self);           \
			}                                                        \
		}                                                            \
		                                                             \
		if (__wait_ret != -EAGAIN) {                                 \
			sched_wait_cleanup_lthread(self);                        \
		}                                                            \
		                                                             \
		__wait_ret;                                                  \
	})

#define SCHED_WAIT_LTHREAD(self, cond_expr) \
	SCHED_WAIT_TIMEOUT_LTHREAD(self, cond_expr, SCHED_TIMEOUT_INFINITE)

#endif /* KERNEL_LTHREAD_SCHED_WAIT_H_ */
