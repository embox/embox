/**
 * @file
 * @brief Implements critical sections API.
 *
 * @details
 *
 @verbatim
        +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 bit#:  |15 | ... |13 |12 ||11 | ... | 9 | 8 || 7 | ... | 1 | 0 |
 level: | P |    preempt  || P |    softirq  || P |    hardirq  |
        +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 @endverbatim
 *
 * @c hardirq:
 @verbatim
        +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 level: | P |    preempt  || P |    softirq  || P |    hardirq  |
        +---+-----+---+---++---+-----+---+---++---+-----+---+---+
 mask:  |   |     |   |   ||   |     |   |   || * | *** | * | * |
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
 mask:  |   |     |   |   ||   | *** | * | * ||   |     |   |   |
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
 mask:  |   | *** | * | * ||   |     |   |   ||   |     |   |   |
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

/* Critical levels mask. */

#define __CRITICAL_HARDIRQ 0x00ff

#define __CRITICAL_SOFTIRQ 0x0f00

#define __CRITICAL_PREEMPT 0xf000

/* Internal helper macros. */

#define __CRITICAL_MASK(critical) \
	(critical)

/* 01111000
 * 01110111
 * 00001111
 * 00000111
 */
#define __CRITICAL_LOWER(critical) \
	(((critical) ^ ((critical) - 1)) >> 1)

/* 01111000
 * 00000111
 * 01111111
 */
#define __CRITICAL_BELOW(critical) \
	((critical) | __CRITICAL_LOWER(critical))

/* 01111000
 * 00000111
 * 00001000
 */
#define __CRITICAL_COUNT(critical) \
	(__CRITICAL_LOWER(critical) + 1)

/* 01111000
 * 01111111
 * 10000000
 */
#define __CRITICAL_PENDING(critical) \
	(__CRITICAL_BELOW(critical) + 1)

static inline int critical_allows(__critical_t critical) {
	return !(__critical_count_get() & __CRITICAL_BELOW(critical));
}

static inline int critical_inside(__critical_t critical) {
	return __critical_count_get() & __CRITICAL_MASK(critical);
}

static inline int critical_pending(__critical_t critical) {
	return __critical_count_get() & __CRITICAL_PENDING(critical);
}

static inline void critical_enter(__critical_t critical) {
	__critical_count_add(__CRITICAL_COUNT(critical));
}

static inline void critical_leave(__critical_t critical) {
	__critical_count_sub(__CRITICAL_COUNT(critical));
}

extern void __sched_dispatch(void);
extern void softirq_dispatch(void);

static inline int critical_need_dispatch(__critical_t critical) {
	return critical_allows(critical) && critical_pending(critical);
}

static inline void critical_check_pending(__critical_t critical) {
	switch (critical) {
	case __CRITICAL_HARDIRQ:

	case __CRITICAL_SOFTIRQ:
		if (critical_need_dispatch(critical)) {
			__critical_count_clr_bit(__CRITICAL_PENDING (critical));
			softirq_dispatch();
		}

	case __CRITICAL_PREEMPT:
		if (critical_need_dispatch(critical)) {
			__critical_count_clr_bit(__CRITICAL_PENDING (critical));
			__sched_dispatch();
		}
	}
}
#endif /* KERNEL_CRITICAL_API_IMPL_H_ */
