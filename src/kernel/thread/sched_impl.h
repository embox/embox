/**
 * @file
 * @brief TODO documentation for sched_impl.h -- Alina
 *
 * @date 23.03. 2011
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_SCHED_IMPL_H_
#define KERNEL_THREAD_SCHED_IMPL_H_

#include <kernel/critical/api.h>
#include <kernel/thread/sched_policy.h>

struct thread;

static inline struct thread *sched_current(void) {
	return sched_policy_current();
}

static inline void sched_lock(void) {
	critical_enter(CRITICAL_PREEMPT);
}

static inline void sched_unlock_noswitch(void) {
	critical_leave(CRITICAL_PREEMPT);
}

static inline void sched_unlock(void) {
	extern void sched_check_switch(void);
	sched_unlock_noswitch();
	sched_check_switch();
}

#endif /* KERNEL_THREAD_SCHED_IMPL_H_ */
