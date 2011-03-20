/**
 * @file
 * @brief This file is intended for internal usage by critical.h .
 *
 * @details
 *
@verbatim
        +-----+---+---+-----+---+---+-----+---+---+
  bit#: | ... |13 |12 | ... | 9 | 8 | ... | 1 | 0 |
 level: |    preempt  |    softirq  |    hardirq  |
        +-----+---+---+-----+---+---+-----+---+---+
@endverbatim
 *
 * @c hardirq:
@verbatim
        +-----+---+---+-----+---+---+-----+---+---+
 level: |    preempt  |    softirq  |    hardirq  |
        +-----+---+---+-----+---+---+-----+---+---+
  mask: |     |   |   |     |   |   | *** | * | * |
 below: |     |   |   |     |   |   | *** | * | * |
 count: |     |   |   |     |   |   |     |   | * |
        +-----+---+---+-----+---+---+-----+---+---+
@endverbatim
 *
 * @c softirq:
@verbatim
        +-----+---+---+-----+---+---+-----+---+---+
 level: |    preempt  |    softirq  |    hardirq  |
        +-----+---+---+-----+---+---+-----+---+---+
  mask: |     |   |   | *** | * | * |     |   |   |
 below: |     |   |   | *** | * | * | *** | * | * |
 count: |     |   |   |     |   | * |     |   |   |
        +-----+---+---+-----+---+---+-----+---+---+
@endverbatim
 *
 * @c preempt:
@verbatim
        +-----+---+---+-----+---+---+-----+---+---+
 level: |    preempt  |    softirq  |    hardirq  |
        +-----+---+---+-----+---+---+-----+---+---+
  mask: | *** | * | * |     |   |   |     |   |   |
 below: | *** | * | * | *** | * | * | *** | * | * |
 count: |     |   | * |     |   |   |     |   |   |
        +-----+---+---+-----+---+---+-----+---+---+
@endverbatim
 *
 * @date 23.05.2010
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_H_
# error "Do not include this file directly, use <kernel/critical.h> instead!"
#endif /* KERNEL_CRITICAL_H_ */

#include "critical_config.h"

#define __CRITICAL_VALUE(bits, lower_mask) \
	((__CRITICAL_BELOW(lower_mask) << (bits) | CRITICAL_VALUE_LOWEST(bits)) \
			^ __CRITICAL_BELOW(lower_mask))

#define __CRITICAL_VALUE_LOWEST(bits) \
	((0x1UL << (bits)) - 1)

/* Internal helper macros. */

#define __CRITICAL_LOWER(critical) \
	(((critical) ^ ((critical) - 1)) >> 1)

#define __CRITICAL_MASK(critical) \
	(critical)

#define __CRITICAL_BELOW(critical) \
	((critical) | __CRITICAL_LOWER(critical))

#define __CRITICAL_COUNT(critical) \
	(__CRITICAL_LOWER(critical) + 1)
