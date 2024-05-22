/**
 * @file
 *
 * @date    19.06.2020
 * @author  Alexander Kalmuk
 */

#ifndef ARCH_ARM_ARMMLIB_IRQ_STATIC_IMPL_H_
#define ARCH_ARM_ARMMLIB_IRQ_STATIC_IMPL_H_

#include <util/macro.h>
#include <kernel/irq_stack.h>

#define IRQ_HND_NAME(_hnd) \
	MACRO_CONCAT(_hnd,_entry)

#if !defined(__ARM_ARCH_6M__) /* FIXME unsupported for Cortex-M0 */
#define STATIC_IRQ_HANDLER_DEF(_irq_nr, _static_hnd, _hnd, _data)     \
	void IRQ_HND_NAME(_static_hnd)(void) {                            \
		critical_enter(CRITICAL_IRQ_HANDLER);                         \
		_hnd(_irq_nr, _data);                                         \
		critical_leave(CRITICAL_IRQ_HANDLER);                         \
	}                                                                 \
	__attribute__((naked)) void _static_hnd(void) {                   \
		__asm__ __volatile__ (                                        \
			"    push {lr}                                     \n"    \
			"    bl "MACRO_STRING(IRQ_HND_NAME(_static_hnd))"  \n"    \
			"    pop {lr}                                      \n"    \
			"    b arm_m_irq_exit                              \n"    \
			: : :                                                     \
		);                                                            \
	}
#else
#define STATIC_IRQ_HANDLER_DEF(_irq_nr, _static_hnd, _hnd, _data)     \
	void _static_hnd(void) {                                          \
		/* critical_enter(CRITICAL_IRQ_HANDLER); */                   \
		_hnd(_irq_nr, _data);                                         \
		/* critical_leave(CRITICAL_IRQ_HANDLER); */                   \
	}
#endif /* !defined(__ARM_ARCH_6M__) */

#endif /* ARCH_ARM_ARMMLIB_IRQ_STATIC_IMPL_H_ */
