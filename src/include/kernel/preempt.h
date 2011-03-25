/**
 * @file
 * @brief TODO
 *
 * @date 23.05.2010
 * @author Eldar Abusalimov
 */

#ifndef PREEMPT_H_
#define PREEMPT_H_

#include <kernel/critical/api.h>

inline static void preempt_disable(void) {
	critical_enter(CRITICAL_PREEMPT);
}

inline static void preempt_enable_noresched(void) {
	critical_leave(CRITICAL_PREEMPT);
}

inline static void preempt_check_resched(void) {
	if (critical_allows(CRITICAL_PREEMPT)) {
		sched_invoke();
	}
}

inline static void preempt_enable(void) {
	preempt_enable_noresched();
	preempt_check_resched();
}

#endif /* PREEMPT_H_ */
