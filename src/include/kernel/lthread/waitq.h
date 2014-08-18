/**
 * @file
 * @brief Defines lthread-specific methods associated with waitq. You can also
 * or instead use ones from schedee/waitq.h
 *
 * @date 7.08.2014
 * @author Vita Loginova
 */

#ifndef KERNEL_LTHREAD_WAITQ_H_
#define KERNEL_LTHREAD_WAITQ_H_

#include <kernel/sched/waitq.h>
#include <kernel/schedee/current.h>

/**
 * Waits without timeout till cond_expr becomes true.
 *
 * @param wq if cond_expr is false, puts lthread in wq
 * @param cond_expr conditional expression waiting to be true
 *
 * @return waiting result
 * @retval 0 cond_expr is true
 * @retval -EAGAIN lthread placed in wq, lthread has to finish its routine
 *                 in order to be waken up by wq.
 */
#define WAITQ_WAIT_LTHREAD(wq, cond_expr) \
	({ \
		int ret = 0; \
		struct waitq_link *wql = &schedee_get_current()->waitq_link; \
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
