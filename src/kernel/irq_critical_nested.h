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
	critical_enter(CRITICAL_IRQ_HANDLER);
}

static inline void irq_nested_unlock(void) {
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

#endif /* KERNEL_IRQ_NESTED_H_ */
