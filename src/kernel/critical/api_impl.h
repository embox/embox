/**
 * @file
 * @brief Implements critical sections API.
 *
 * @details
 *
 @verbatim
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 bit#: |15 | ... |13 |12 ||11 | ... | 9 | 8 || 7 | ... | 1 | 0 |
 level: | P |    preempt  || P |    softirq  || P |    hardirq  |
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 @endverbatim
 *
 * @c hardirq:
 @verbatim
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 level: | P |    preempt  || P |    softirq  || P |    hardirq  |
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 mask: |   |     |   |   ||   |     |   |   || * | *** | * | * |
 below: |   |     |   |   ||   |     |   |   || * | *** | * | * |
 count: |   |     |   |   ||   |     |   |   ||   |     |   | * |
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 @endverbatim
 *
 * @c softirq:
 @verbatim
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 level: | P |    preempt  || P |    softirq  || P |    hardirq  |
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 mask: |   |     |   |   ||   | *** | * | * ||   |     |   |   |
 below: |   |     |   |   ||   | *** | * | * || * | *** | * | * |
 count: |   |     |   |   ||   |     |   | * ||   |     |   |   |
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 @endverbatim
 *
 * @c preempt:
 @verbatim
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 level: | P |    preempt  || P |    softirq  || P |    hardirq  |
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 mask: |   | *** | * | * ||   |     |   |   ||   |     |   |   |
 below: |   | *** | * | * || * | *** | * | * || * | *** | * | * |
 count: |   |     |   | * ||   |     |   |   ||   |     |   |   |
 +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 @endverbatim
 *
 * @date 16.05.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_API_IMPL_H_
#define KERNEL_CRITICAL_API_IMPL_H_

#include "count.h"
#include "levels.h"
#include "dispatch.h"

/* Internal helper macros. */

#define __CRITICAL_LOWER(critical) \
	(((critical) ^ ((critical) - 1)) >> 1)

#define __CRITICAL_MASK(critical) \
	(critical)

#define __CRITICAL_BELOW(critical) \
	((critical) | __CRITICAL_LOWER(critical))

#define __CRITICAL_COUNT(critical) \
	(__CRITICAL_LOWER(critical) + 1)

static inline int critical_allows(__critical_t critical) {
	return !(__critical_count_get() & __CRITICAL_BELOW(critical));
}

static inline int critical_inside(__critical_t critical) {
	return __critical_count_get() & __CRITICAL_MASK(critical);
}

static inline void critical_enter(__critical_t critical) {
	__critical_count_add(__CRITICAL_COUNT(critical));
}

static inline void critical_leave(__critical_t critical) {
	__critical_count_sub(__CRITICAL_COUNT(critical));
}

#define __critical_dispatch(critical) \
	((critical) == __CRITICAL_HARDIRQ ? __critical_dispatch_hardirq() : \
	 (critical) == __CRITICAL_SOFTIRQ ? __critical_dispatch_softirq() : \
	 (critical) == __CRITICAL_PREEMPT ? __critical_dispatch_preempt() : \
			__critical_dispatch_error())

static long i = 1 << 15;

/**
 * verify: is 15 is 0 or 1.
 * @see details
 * @param critical 4 bytes with pending bits to verify
 */
static inline void critical_softirq_check_pending(__critical_t critical) {
	if (critical & i) {
		critical = critical & ~i;
		//dispatch
	}
}

/**
 * verify: is 15 and 9 bits are 0 or 1.
 * @see details
 * @param critical 4 bytes with pending bits to verify
 */
static inline void critical_irq_check_pending(__critical_t critical) {
	if ((critical >> 8) & 1) {
		critical = critical & ~(1 << 8);
		//dispatch
	}
	critical_softirq_check_pending(critical);
}

/**
 *
 * @param critical 4 bytes with pending bits to verify
 */
static inline void critical_sched_check_pending(__critical_t critical) {
      //dispatch
}
#endif /* KERNEL_CRITICAL_API_IMPL_H_ */
