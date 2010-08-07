/**
 * @file
 * @brief TODO
 *
 * @date 23.05.2010
 * @author Eldar Abusalimov
 */

#ifndef PREEMPT_H_
#define PREEMPT_H_

#include <kernel/critical.h>

inline static void preempt_disable(void) {
	critical_enter_preempt();
}

inline static void preempt_enable_noresched(void) {
	critical_leave_preempt();
}

inline static void preempt_check_resched(void) {
	if (critical_allows_preempt()) {
		sched_invoke();
	}
}

inline static void preempt_enable(void) {
	preempt_enable_noresched();
	preempt_check_resched();
}

#endif /* PREEMPT_H_ */
