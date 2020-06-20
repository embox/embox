/**
 * @file
 *
 * @date    19.06.2020
 * @author  Alexander Kalmuk
 */

#ifndef ARCH_ARM_ARMMLIB_IRQ_STATIC_IMPL_H_
#define ARCH_ARM_ARMMLIB_IRQ_STATIC_IMPL_H_

#include <assert.h>
#include <kernel/irq_stack.h>

/* It declares a "naked" function - that is, it doesn't include prologue
 * or epilogue. After execution of irq handler, arm_m_irq_exit() will be
 * called to check properly whether some pending work is required, and
 * do this work if it's required.
 */

#if !defined(__ARM_ARCH_6M__) /* FIXME unsupported for Cortex-M0 */
#define STATIC_IRQ_HANDLER_DEF(_irq_nr, _static_hnd, _hnd, _data)     \
	__attribute__((naked)) void _static_hnd(void) {                   \
		__asm__ __volatile__ (                                        \
			"    push {lr}            \n"                             \
		);                                                            \
		assertf(irq_stack_protection() == 0,                          \
			"Stack overflow detected on irq handler %s\n", __func__); \
		critical_enter(CRITICAL_IRQ_HANDLER);                         \
		_hnd(_irq_nr, _data);                                         \
		critical_leave(CRITICAL_IRQ_HANDLER);                         \
		__asm__ __volatile__ (                                        \
			"    pop {lr}             \n"                             \
			"    b arm_m_irq_exit     \n"                             \
		);                                                            \
	}
#else
#define STATIC_IRQ_HANDLER_DEF(_irq_nr, _static_hnd, _hnd, _data)     \
	void _static_hnd(void) {                                          \
		assertf(irq_stack_protection() == 0,                          \
			"Stack overflow detected on irq handler %s\n", __func__); \
		critical_enter(CRITICAL_IRQ_HANDLER);                         \
		_hnd(_irq_nr, _data);                                         \
		critical_leave(CRITICAL_IRQ_HANDLER);                         \
	}
#endif /* !defined(__ARM_ARCH_6M__) */

#endif /* ARCH_ARM_ARMMLIB_IRQ_STATIC_IMPL_H_ */
