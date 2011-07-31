/**
 *  @brief
 *  @details
 *  @date 13.07.11
 *  @author Alexandr Kalmuk
 */

#ifndef KERNEL_IRQ_NESTED_H_
#define KERNEL_IRQ_NESTED_H_

#include <kernel/critical/api.h>

static inline void irq_nested_lock(void) {
	critical_enter(__CRITICAL_IRQ_NESTED);
}

static inline void irq_nested_unlock(void) {
	critical_leave(__CRITICAL_IRQ_NESTED);
	critical_check_pending(__CRITICAL_IRQ_NESTED);
}

#endif /* KERNEL_IRQ_NESTED_H_ */
