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

/* Valid critical level mask is a single contiguous block of set bits. */
#define __CRITICAL_CHECK_BIT_BLOCK(level) \
	(__CRITICAL_SOFTER(level) | (level) | __CRITICAL_HARDER(level))

#if ~0 != \
	  __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_IRQ_LOCK)        \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_IRQ_HANDLER)     \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SOFTIRQ_LOCK)    \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SOFTIRQ_HANDLER) \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SOFTIRQ_PENDING) \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SCHED_LOCK)      \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SCHED_PENDING)
# error "CRITICAL_XXX must contain a single contiguous block of bits"
#endif

/* Pending bit mask is a power of two. */
#define __CRITICAL_CHECK_PENDING(level) \
	!((level) & ((level) - 1))

#if	   !__CRITICAL_CHECK_PENDING(CRITICAL_SOFTIRQ_PENDING) \
	|| !__CRITICAL_CHECK_PENDING(CRITICAL_SCHED_PENDING)
# error "CRITICAL_XXX_PENDING must be a power of two (only one single bit)"
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

static inline int __critical_pending_test_and_clear(__critical_t level,
		__critical_t pending) {
	/* Don't check pending state for levels harder then the current one.
	 * For example, we don't need to check for pending softirq's when unlocking
	 * the scheduler.
	 * This test only involves values known at compile time so actually this
	 * check will be optimized out (well, if constant propagation is performed
	 * honestly which is true at least for modern GCC's). */
	if (pending & __CRITICAL_HARDER(level)) {
		return 0;
	}
	/* In case when pending bit is not set (nothing to dispatch) or there are
	 * any lower bits set (that means being inside a too hard context)
	 * we can't take any action and, well, we don't. */
	if ((pending ^ __critical_count_get()) & (pending ^ (pending - 1))) {
		return 0;
	}

	__critical_count_clr_bit(pending);
	return 1;
}

static inline void critical_check_dispatch(__critical_t level) {
	// TODO replace with macros or inlined calls. -- Eldar
	extern void __attribute__ ((weak)) __sched_dispatch(void);
	extern void __attribute__ ((weak)) softirq_dispatch(void);

	if (__critical_pending_test_and_clear(level, CRITICAL_SOFTIRQ_PENDING)) {
		softirq_dispatch();
	}
	if (__critical_pending_test_and_clear(level, CRITICAL_SCHED_PENDING)) {
		__sched_dispatch();
	}
}

static inline void critical_request_dispatch(__critical_t level) {
	__critical_set_pending(level);
	critical_check_dispatch(level);
}

#endif /* KERNEL_CRITICAL_API_IMPL_H_ */
