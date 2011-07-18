/**
 *  @brief Implements sched locks interface.
 *  @details This header in use also if mod 'sched' is not enable.
 *  @date 13.07.11
 *  @author Alexandr Kalmuk
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

/**
 * Try to call sched_dispath().
 *
 * sched_dispath() will be called only outside softirq,irq and sched locks:
 * lock();
 *        ...
 * unlock();
 *
 * Else sched_dispatch() will delay and then will call immediately
 * after outermost sched_unlock(), softirq_unlock() or irq_unlock();
 */
void sched_try_dispatch(void) {
	if (critical_allows(__CRITICAL_PREEMPT)) {
		__sched_dispatch();
	} else
		__critical_count_set_bit(__CRITICAL_PREEMPT);
}

#endif /* KERNEL_THREAD_SCHED_CRITICAL_H_ */
