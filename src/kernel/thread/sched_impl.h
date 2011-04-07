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

inline static struct thread *sched_current(void) {
	return sched_policy_current();
}

inline static void sched_lock(void) {
	critical_enter(CRITICAL_PREEMPT);
}

inline static void sched_unlock_noswitch(void) {
	critical_leave(CRITICAL_PREEMPT);
}

inline static void sched_check_switch(void) {
	extern void __sched_dispatch(void);
	if (critical_allows(CRITICAL_PREEMPT) && sched_current()->reschedule) {
		__sched_dispatch();
	}
}

inline static void sched_unlock(void) {
	sched_unlock_noswitch();
	sched_check_switch();
}

#endif /* KERNEL_THREAD_SCHED_IMPL_H_ */
