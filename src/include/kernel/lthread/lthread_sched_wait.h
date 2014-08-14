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

/* Values for sched_wait_info->status */
#define SCHED_WAIT_FINISHED 0
#define SCHED_WAIT_STARTED 1

#define SCHED_WAIT_TIMEOUT(cond_expr) \
	SCHED_WAIT_TIMEOUT_LTHREAD(cond_expr, SCHED_TIMEOUT_INFINITE)

/**
 * Waits with timeout till cond_expr becomes true.
 *
 * @param cond_expr conditional expression waiting to be true
 *
 * @return waiting result
 * @retval 0 cond_expr is true.
 * @retval -EAGAIN lthread has to finish its routine in order to be waken up.
 * @retval -ETIMEDOUT Waiting finished. Timeout is exceeded.
 */
#define SCHED_WAIT_TIMEOUT_LTHREAD(cond_expr, timeout) \
	({                                                               \
		int __wait_ret = 0;                                          \
		clock_t __wait_timeout = timeout == SCHED_TIMEOUT_INFINITE ? \
			SCHED_TIMEOUT_INFINITE : ms2jiffies(timeout);            \
		                                                             \
		if (!(cond_expr)) {                                          \
			sched_wait_prepare_lthread(__wait_timeout);              \
			                                                         \
			__wait_ret = sched_wait_timeout_lthread();               \
			                                                         \
			if (__wait_ret == 0) {                                   \
			__wait_ret = sched_wait_timeout_lthread();               \
			}                                                        \
		}                                                            \
		                                                             \
		if (__wait_ret != -EAGAIN) {                                 \
			sched_wait_cleanup_lthread();                            \
		}                                                            \
		                                                             \
		__wait_ret;                                                  \
	})


struct sched_wait_info {
	struct sys_timer *tmr;
	clock_t remain;
	clock_t cur_time;
	clock_t prev_time;
	int status;
};

extern void sched_wait_info_init(struct sched_wait_info *info);

extern void sched_wait_prepare_lthread(clock_t timeout);
extern void sched_wait_cleanup_lthread(void);

/**
 * Waits while timeout is exceeded. Timeout is setted in
 * sched_wait_prepare_lthread.
 *
 * @return waiting result
 * @retval 0 Waiting finished before timeout is exceeded.
 * @retval -EAGAIN Waiting started. lthread has to finish its routine
 *                 in order to be waken up by some schedee or when timeout
 *                 is exceeded.
 * @retval -ETIMEDOUT Waiting finished. Timeout is exceeded.
 */
extern int sched_wait_timeout_lthread(void);

#endif /* KERNEL_LTHREAD_SCHED_WAIT_H_ */
