/**
 *  @brief Implements scheduler interface.
 *  @details This header in use if and only if
 *           mod 'irq' is not enable.
 *
 *  @date 13.07.11
 */

#ifndef KERNEL_THREAD_SCHED_CRITICAL_H_
#define KERNEL_THREAD_SCHED_CRITICAL_H_

#include <kernel/critical/api.h>
#include <kernel/thread/sched.h>

inline void sched_lock(void) {
	critical_enter(CRITICAL_PREEMPT);
}

inline void sched_unlock(void) {
	sched_unlock_noswitch();
	sched_check_switch();
}

inline void sched_try_dispatch(void) {
	if (critical_allows(__CRITICAL_PREEMPT)) {
		__sched_dispatch();
	} else
		__critical_count_set_bit(__CRITICAL_PREEMPT);

}

#endif /* KERNEL_THREAD_SCHED_CRITICAL_H_ */
