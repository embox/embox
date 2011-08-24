/**
 * @file
 * @brief Implements critical sections API.
 *
 * @details
 *
 @verbatim
 ------   --  -- --- --   -- --- --   --  -- --- --   -- --- --   -- --- --
 level    DP  sched_lck   swirq_hnd   DP  swirq_lck   hwirq_hnd   hwirq_lck
 bit_nr   31  30 ... 25   24 ... 19   18  17 ... 12   11 ...  6    5 ...  0
 ------   --  -- --- --   -- --- --   --  -- --- --   -- --- --   -- --- --
 @endverbatim
 *
 * For example, bit masks of the level corresponding to hardware interrupt
 * locks are the following:
 @verbatim
 ------   --  -- --- --   -- --- --   --  -- --- --   -- --- --   -- --- --
 level    DP  sched_lck   swirq_hnd   DP  swirq_lck   hwirq_hnd   hwirq_lck
 bit_nr   31  30 ... 25   24 ... 19   18  17 ... 12   11 ...  6    5 ...  0
 ------   --  -- --- --   -- --- --   --  -- --- --   -- --- --   -- --- --
 mask                                                              *  **  *
 harder
 softer    *   *  **  *    *  **  *    *   *  **  *    *  **  *
 count                                                                    *
 ------   --  -- --- --   -- --- --   --  -- --- --   -- --- --   -- --- --
 @endverbatim
 *
 * And masks being used when entering/leaving software interrupt handlers look
 * like:
 @verbatim
 ------   --  -- --- --   -- --- --   --  -- --- --   -- --- --   -- --- --
 level    DP  sched_lck   swirq_hnd   DP  swirq_lck   hwirq_hnd   hwirq_lck
 bit_nr   31  30 ... 25   24 ... 19   18  17 ... 12   11 ...  6    5 ...  0
 ------   --  -- --- --   -- --- --   --  -- --- --   -- --- --   -- --- --
 mask                      *  **  *
 harder                                *   *  **  *    *  **  *    *  **  *
 softer    *   *  **  *
 count                            *
 ------   --  -- --- --   -- --- --   --  -- --- --   -- --- --   -- --- --
 @endverbatim
 *
 * @date 16.05.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_API_IMPL_H_
#define KERNEL_CRITICAL_API_IMPL_H_

#include <assert.h>

#include "count.h"

/* Internal helper macros. */

/* 01111000 *
 * 01110111 *
 * 00001111 *
 * 00000111 */
#define __CRITICAL_HARDER(level) \
	(((level) ^ ((level) - 1)) >> 1)

/* 01111000 *
 * 10000111 *
 * 00000111 *
 * 10000000 */
#define __CRITICAL_SOFTER(level) \
	(~(level) ^ __CRITICAL_HARDER(level))

/* 01111000 *
 * 00000111 *
 * 00001000 */
#define __CRITICAL_COUNT(level) \
	(__CRITICAL_HARDER(level) + 1)

/* 01111000 *
 * 00000111 *
 * 01111111 *
 * 10000000 */
#define __CRITICAL_PENDING(level) \
	((level | __CRITICAL_HARDER(level)) + 1)

/* Check for defined masks values. */
#define __CRITICAL_CHECK \
	  __CRITICAL_CHECK_EACH(CRITICAL_IRQ_LOCK)        \
	& __CRITICAL_CHECK_EACH(CRITICAL_IRQ_HANDLER)     \
	& __CRITICAL_CHECK_EACH(CRITICAL_SOFTIRQ_LOCK)    \
	& __CRITICAL_CHECK_EACH(CRITICAL_SOFTIRQ_HANDLER) \
	& __CRITICAL_CHECK_EACH(CRITICAL_SOFTIRQ_PENDING) \
	& __CRITICAL_CHECK_EACH(CRITICAL_SCHED_LOCK)      \
	& __CRITICAL_CHECK_EACH(CRITICAL_SCHED_PENDING)   \
		== ~0
#define __CRITICAL_CHECK_EACH(level) \
	(__CRITICAL_SOFTER(level) | (level) | __CRITICAL_HARDER(level))

#if !__CRITICAL_CHECK
# error "Bad CRITICAL_XXX masks"
#endif

static inline int critical_allows(__critical_t level) {
	return !(__critical_count_get() & (level | __CRITICAL_HARDER(level)));
}

static inline int critical_inside(__critical_t level) {
	return __critical_count_get() & level;
}

static inline void critical_enter(__critical_t level) {
	__critical_count_add(__CRITICAL_COUNT(level));
}

static inline void critical_leave(__critical_t level) {
	__critical_count_sub(__CRITICAL_COUNT(level));
}

static inline void __critical_set_pending(__critical_t level) {
	__critical_count_set_bit(__CRITICAL_PENDING(level));
}

static inline void __critical_clr_pending(__critical_t level) {
	__critical_count_clr_bit(__CRITICAL_PENDING(level));
}

static inline int critical_pending_dispatch(__critical_t level) {
	return __critical_count_get() & __CRITICAL_PENDING(level);
}

static inline int __critical_need_dispatch(__critical_t level) {
	int need = critical_allows(level) && critical_pending_dispatch(level);
	if (need) {
	}
	return need;
}

// TODO replace with macros or inlined calls. -- Eldar
extern void __attribute__ ((weak)) __sched_dispatch(void);
extern void __attribute__ ((weak)) softirq_dispatch(void);

#define __critical_check_pending(level, pending, dispatch_fn) \
	if (pending & __CRITICAL_SOFTER(level)) {       \
		if (pending == (~__CRITICAL_SOFTER(pending) \
				& __critical_count_get())) {        \
			__critical_count_clr_bit(pending);      \
			assert(dispatch_fn);                    \
			dispatch_fn();                          \
		}                                           \
	}

#define critical_check_dispatch(level) \
	do {                                                     \
		__critical_check_pending(level,                      \
				CRITICAL_SOFTIRQ_PENDING, softirq_dispatch); \
		__critical_check_pending(level,                      \
				CRITICAL_SCHED_PENDING, __sched_dispatch);   \
	} while(0)

static inline void critical_request_dispatch(__critical_t level) {
	__critical_set_pending(level);
	critical_check_dispatch(level);

#if 0
	if (critical_allows(level)) {
		// TODO
	} else {
		__critical_set_pending(level);
	}
#endif
}

#endif /* KERNEL_CRITICAL_API_IMPL_H_ */
