/**
 * @file
 * @brief Implements critical sections API.
 *
 * @details
 *
 @verbatim
        +---+-----+---+---++---+-----+----++---+-----+---++---+-----+---+
 bit#:  |19 | ... |17 |16 ||15 | ... | 12 ||11 | ... | 8 || 7 | ... | 0 |
 level: | P | sched_lock  || P | softirq  ||     irq     ||  irq_lock   |
        +---+-----+---+---++---+-----+----++---+-----+---++---+-----+---+
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

#include <assert.h>

#include "count.h"

/* Critical levels mask. */

#define __CRITICAL_HARDIRQ     0x000ff
#define __CRITICAL_IRQ_NESTED  0x00f00
#define __CRITICAL_SOFTIRQ     0x07000  /* One bit room for pending state. */
#define __CRITICAL_PREEMPT     0x70000  /* One bit room for pending state. */

/* Internal helper macros. */

#define __CRITICAL_MASK(level) \
	(level)

/* 01111000
 * 01110111
 * 00001111
 * 00000111
 */
#define __CRITICAL_LOWER(level) \
	(((level) ^ ((level) - 1)) >> 1)

/* 01111000
 * 00000111
 * 01111111
 */
#define __CRITICAL_BELOW(level) \
	((level) | __CRITICAL_LOWER(level))

/* 01111000
 * 00000111
 * 00001000
 */
#define __CRITICAL_COUNT(level) \
	(__CRITICAL_LOWER(level) + 1)

/* 01111000
 * 01111111
 * 10000000
 */
#define __CRITICAL_PENDING(level) \
	(__CRITICAL_BELOW(level) + 1)

static inline int critical_allows(__critical_t level) {
	return !(__critical_count_get() & __CRITICAL_BELOW(level));
}

static inline int critical_inside(__critical_t level) {
	return __critical_count_get() & __CRITICAL_MASK(level);
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
		__critical_clr_pending(level);
	}
	return need;
}

static inline void critical_check_dispatch(__critical_t level) {
	extern void __attribute__ ((weak)) __sched_dispatch(void);
	extern void __attribute__ ((weak)) softirq_dispatch(void);

	/* Switch below is not the most optimal way to dispatch pending,
	 * but for now it is ok. */
	switch (level) {

	case __CRITICAL_HARDIRQ:
	case __CRITICAL_IRQ_NESTED:
	case __CRITICAL_SOFTIRQ:
		if (__critical_need_dispatch(__CRITICAL_SOFTIRQ)) {
			assert(softirq_dispatch);
			softirq_dispatch();
		}
		/* FALLTHROUGH */

	case __CRITICAL_PREEMPT:
		if (__critical_need_dispatch(__CRITICAL_PREEMPT)) {
			assert(__sched_dispatch);
			__sched_dispatch();
		}
		break;

	default:
		assert(0);
	}
}

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
