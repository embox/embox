/**
 * @file
 * @brief This file is intended for internal usage by critical.h .
 *
 * @date 23.05.2010
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_H_
# error "Do not include this file directly, use <kernel/critical.h> instead!"
#endif /* KERNEL_CRITICAL_H_ */

#include "critical_config.h"

/* Levels order. */

#define __CRITICAL_HARDIRQ_SHIFT \
		(__CRITICAL_SOFTIRQ_SHIFT + __CRITICAL_SOFTIRQ_BITS)
#define __CRITICAL_SOFTIRQ_SHIFT \
		(__CRITICAL_PREEMPT_SHIFT + __CRITICAL_PREEMPT_BITS)
#define __CRITICAL_PREEMPT_SHIFT 0

/* internal helper macros. */

#define __CRITICAL_MASK(name) \
		__CRITICAL_MASK__(__CRITICAL_##name##_BITS, __CRITICAL_##name##_SHIFT)
#define __CRITICAL_MASK__(bits, shift) (((0x1UL << (bits)) - 1) << (shift))

#define __CRITICAL_HIGH(name) \
		__CRITICAL_HIGH__(__CRITICAL_##name##_BITS, __CRITICAL_##name##_SHIFT)
#define __CRITICAL_HIGH__(bits, shift) (~((0x1UL << (shift)) - 1))

#define __CRITICAL_COUNT(name) \
		__CRITICAL_COUNT__(__CRITICAL_##name##_SHIFT)
#define __CRITICAL_COUNT__(shift) (1UL << (shift))

/* All of the following values are __CRITICAL_XXX_BITS derivatives. */

#define __CRITICAL_PREEMPT_MASK  __CRITICAL_MASK(PREEMPT)
#define __CRITICAL_SOFTIRQ_MASK  __CRITICAL_MASK(SOFTIRQ)
#define __CRITICAL_HARDIRQ_MASK  __CRITICAL_MASK(HARDIRQ)

#define __CRITICAL_PREEMPT_HIGH  __CRITICAL_HIGH(PREEMPT)
#define __CRITICAL_SOFTIRQ_HIGH  __CRITICAL_HIGH(SOFTIRQ)
#define __CRITICAL_HARDIRQ_HIGH  __CRITICAL_HIGH(HARDIRQ)

#define __CRITICAL_PREEMPT_COUNT __CRITICAL_COUNT(PREEMPT)
#define __CRITICAL_SOFTIRQ_COUNT __CRITICAL_COUNT(SOFTIRQ)
#define __CRITICAL_HARDIRQ_COUNT __CRITICAL_COUNT(HARDIRQ)

