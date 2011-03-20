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

inline static int critical_allows(__critical_t critical) {
	// TODO hmm, check it. -- Eldar
	return !(__critical_count_get()
			& (__CRITICAL_HIGH(critical) | __CRITICAL_MASK(critical)));
}

inline static int critical_inside(__critical_t critical) {
	return __critical_count_get() & __CRITICAL_MASK(critical);
}

inline static void critical_enter(__critical_t critical) {
	__critical_count_add(__CRITICAL_COUNT(critical));
}

inline static void critical_leave(__critical_t critical) {
	__critical_count_sub(__CRITICAL_COUNT(critical));
}

