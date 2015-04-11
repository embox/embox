/**
 * @file
 * @brief Defines lthread-specific methods associated with waitq. You can also
 * or instead use ones from sched/waitq.h
 *
 * @date 7.08.2014
 * @author Vita Loginova
 */

#ifndef KERNEL_LTHREAD_WAITQ_H_
#define KERNEL_LTHREAD_WAITQ_H_

#include <kernel/sched/waitq.h>

/**
 * Waits without timeout till @p cond_expr becomes true.
 *
 * @param wq
 *   If cond_expr is false, puts lthread in wq.
 * @param cond_expr
 *   Conditional expression waiting to be true.
 *
 * @return
 *   Waiting result.
 * @retval 0
 *   In case cond_expr is true.
 * @retval -EAGAIN
 *   The current lthread is placed in @p wq, lthread has to finish its routine
 *   in order to be waken up later.
 */
#define WAITQ_WAIT_LTHREAD(self, wq, cond_expr) \
	({ \
		int ret = 0; \
		struct waitq_link *wql = &self->schedee.waitq_link; \
		if (!wql->schedee) { \
			waitq_link_init(wql); \
		} \
		if (cond_expr) { \
			waitq_wait_cleanup(wq, wql); \
		} else { \
			waitq_wait_prepare(wq, wql); \
			ret = -EAGAIN; \
		} \
		ret; \
	})

#endif /* KERNEL_LTHREAD_WAITQ_H_ */
