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
#include <hal/ipl.h>

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

/* Valid critical level mask is a single contiguous block of set bits. */
#define __CRITICAL_CHECK_BIT_BLOCK(level) \
	(__CRITICAL_SOFTER(level) | (level) | __CRITICAL_HARDER(level))

#if ~0 != \
	  __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_IRQ_LOCK)        \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_IRQ_HANDLER)     \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SOFTIRQ_LOCK)    \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SOFTIRQ_HANDLER) \
	& __CRITICAL_CHECK_BIT_BLOCK(CRITICAL_SCHED_LOCK)
# error "CRITICAL_XXX must contain a single contiguous block of bits"
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

#endif /* KERNEL_CRITICAL_API_IMPL_H_ */
