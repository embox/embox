/**
 * @file
 * @brief TODO
 *
 * @date Apr 9, 2011
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_LEVELS_H_
#define KERNEL_CRITICAL_LEVELS_H_

#include <kernel/thread/preempt_critical.h>

#define __CRITICAL_VALUE(bits, lower_mask) \
	((__CRITICAL_BELOW(lower_mask) << (bits) | __CRITICAL_VALUE_LOWEST(bits)) \
			^ __CRITICAL_BELOW(lower_mask))

#define __CRITICAL_VALUE_LOWEST(bits) \
	((0x1UL << (bits)) - 1)

/* Critical levels mask. */

#define __CRITICAL_HARDIRQ \
	__CRITICAL_VALUE_LOWEST(8)              /* 0x00ff */

#define __CRITICAL_SOFTIRQ \
	__CRITICAL_VALUE(4, __CRITICAL_HARDIRQ) /* 0x0f00 */

#define __CRITICAL_PREEMPT \
	__CRITICAL_VALUE(PREEMPT_CRITICAL_BITS, __CRITICAL_SOFTIRQ) /* 0xf000 */


#endif /* KERNEL_CRITICAL_LEVELS_H_ */
