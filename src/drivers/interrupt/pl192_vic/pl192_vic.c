/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 29.08.23
 */
#include <stddef.h>
#include <assert.h>
#include <stdint.h>

#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/critical.h>
#include <drivers/irqctrl.h>
#include <drivers/common/memory.h>
#include <framework/mod/options.h>

#include "pl192_vic.h"

#define BASE_ADDR OPTION_GET(NUMBER, base_addr)

#define VIC_IRQ_STATUS       (BASE_ADDR + 0x00)
#define VIC_FIQ_STATUS       (BASE_ADDR + 0x04)
#define VIC_RAW_STATUS       (BASE_ADDR + 0x08)
#define VIC_INT_SELECT       (BASE_ADDR + 0x0c) /* 1 = FIQ, 0 = IRQ */
#define VIC_INT_ENABLE       (BASE_ADDR + 0x10) /* 1 = enable, 0 = no effect */
#define VIC_INT_ENABLE_CLEAR (BASE_ADDR + 0x14) /* 1 = disable, 0 = no effect */
#define VIC_INT_SOFT         (BASE_ADDR + 0x18)
#define VIC_INT_SOFT_CLEAR   (BASE_ADDR + 0x1c)
#define VIC_PROTECT          (BASE_ADDR + 0x20)
#define VIC_VECT_ADDR_BASE   (BASE_ADDR + 0x100)
#define VIC_VECT_PRIOR_BASE  (BASE_ADDR + 0x200)
#define VIC_VECT_ADDR(x)     (VIC_VECT_ADDR_BASE + (4 * (x)))  /* (0..31) */
#define VIC_VECT_PRIOR(x)    (VIC_VECT_PRIOR_BASE + (4 * (x))) /* (0..31) */
#define VIC_ADDR             (BASE_ADDR + 0xf00) /* Currently active ISR */

static int vic_ctrl_init(void) {
	int i;

	REG32_STORE(VIC_INT_SELECT, 0);

	REG32_STORE(VIC_INT_ENABLE, 0);
	REG32_STORE(VIC_INT_ENABLE_CLEAR, ~(uint32_t)0);

	REG32_STORE(VIC_INT_SOFT, 0);
	REG32_STORE(VIC_INT_SOFT_CLEAR, ~(uint32_t)0);

	for (i = 0; i < __IRQCTRL_IRQS_TOTAL; i++) {
		REG32_STORE(VIC_VECT_ADDR(i), i);
		REG32_STORE(VIC_VECT_PRIOR(i), 0xf);
	}

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	REG32_STORE(VIC_INT_ENABLE, 1U << irq);
}

void irqctrl_disable(unsigned int irq) {
	REG32_STORE(VIC_INT_ENABLE_CLEAR, 1U << irq);
}

void irqctrl_force(unsigned int irq) {
}

int irqctrl_pending(unsigned int irq) {
	return 0;
}

void irqctrl_eoi(unsigned int irq) {
}

void irqctrl_set_prio(unsigned int interrupt_nr, unsigned int prio) {
	REG32_STORE(VIC_VECT_PRIOR(interrupt_nr), (prio & 0xf));
}

unsigned int irqctrl_get_prio(unsigned int interrupt_nr) {
	return (REG32_LOAD(VIC_VECT_PRIOR(interrupt_nr)) & 0xf);
}

void interrupt_handle(void) {
	unsigned int irq;

	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	/*  Reading VIC_ADDR register updates the hardware priority register of the
	interrupt controller. */
	irq = REG32_LOAD(VIC_ADDR);

	irqctrl_disable(irq);
	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		ipl_enable();

		irq_dispatch(irq);

		ipl_disable();
	}
	irqctrl_enable(irq);
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();

	/* Writing VIC_ADDR register clears the respective interrupt in the internal
	interrupt priority hardware */
	REG32_STORE(VIC_ADDR, irq);
}

void swi_handle(void) {
}

PERIPH_MEMORY_DEFINE(vic0, BASE_ADDR, 0x1000);

IRQCTRL_DEF(vic, vic_ctrl_init);
