/**
 * @file
 * @brief Implements critical sections API
 *
 * @date 16.05.2010
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_H_
# error "Do not include this file directly, use <kernel/critical.h> instead!"
#endif /* KERNEL_CRITICAL_H_ */

#include "critical_count.h"
#include "critical_defs.h"

inline static unsigned int critical_inside_hardirq(void) {
	return __critical_count_get() & __CRITICAL_HARDIRQ_MASK;
}

inline static void critical_enter_hardirq(void) {
	__critical_count_add(__CRITICAL_HARDIRQ_COUNT);
}

inline static void critical_leave_hardirq(void) {
	__critical_count_sub(__CRITICAL_HARDIRQ_COUNT);
}

inline static unsigned int critical_allows_softirq(void) {
	return !(__critical_count_get() & __CRITICAL_SOFTIRQ_HIGH);
}

inline static unsigned int critical_inside_softirq(void) {
	return __critical_count_get() & __CRITICAL_SOFTIRQ_MASK;
}

inline static void critical_enter_softirq(void) {
	__critical_count_add(__CRITICAL_SOFTIRQ_COUNT);
}

inline static void critical_leave_softirq(void) {
	__critical_count_sub(__CRITICAL_SOFTIRQ_COUNT);
}

inline static unsigned int critical_allows_preempt(void) {
	return !(__critical_count_get() & __CRITICAL_PREEMPT_HIGH);
}

inline static unsigned int critical_inside_preempt(void) {
	return __critical_count_get() & __CRITICAL_PREEMPT_MASK;
}

inline static void critical_enter_preempt(void) {
	__critical_count_add(__CRITICAL_PREEMPT_COUNT);
}

inline static void critical_leave_preempt(void) {
	__critical_count_sub(__CRITICAL_PREEMPT_COUNT);
}
