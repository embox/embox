/**
 * @file
 * @brief
 *
 * @date 7.08.2014
 * @author Vita Loginova
 */

#ifndef KERNEL_LTHREAD_WAITQ_H_
#define KERNEL_LTHREAD_WAITQ_H_

#include <kernel/sched/waitq.h>
#include <kernel/schedee/current.h>

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
