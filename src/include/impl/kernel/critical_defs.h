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
 count: |     |   |   |     |   |   |     |   | * |
  high: |     |   |   |     |   |   |     |   |   |
        +-----+---+---+-----+---+---+-----+---+---+
@endverbatim
 *
 * @c softirq:
@verbatim
        +-----+---+---+-----+---+---+-----+---+---+
 level: |    preempt  |    softirq  |    hardirq  |
        +-----+---+---+-----+---+---+-----+---+---+
  mask: |     |   |   | *** | * | * |     |   |   |
 count: |     |   |   |     |   | * |     |   |   |
  high: |     |   |   |     |   |   | *** | * | * |
        +-----+---+---+-----+---+---+-----+---+---+
@endverbatim
 *
 * @c preempt:
@verbatim
        +-----+---+---+-----+---+---+-----+---+---+
 level: |    preempt  |    softirq  |    hardirq  |
        +-----+---+---+-----+---+---+-----+---+---+
  mask: | *** | * | * |     |   |   |     |   |   |
 count: |     |   | * |     |   |   |     |   |   |
  high: |     |   |   | *** | * | * | *** | * | * |
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

/* internal helper macros. */

#define __CRITICAL_MASK(critical) \
	(critical)

#define __CRITICAL_HIGH(critical) \
	((~(critical) ^ (~(critical) + 1)) >> 1)

#define __CRITICAL_COUNT(critical) \
	(__CRITICAL_HIGH(critical) + 1)

