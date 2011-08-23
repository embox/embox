/**
 * @file
 * @brief TODO documentation for sched_impl.h -- Alina
 *
 * @date 23.03.11
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_SCHED_IMPL_H_
#define KERNEL_THREAD_SCHED_IMPL_H_

#include <kernel/thread/sched_policy.h>

struct thread;

static inline struct thread *sched_current(void) {
	return sched_policy_current();
}

#endif /* KERNEL_THREAD_SCHED_IMPL_H_ */
