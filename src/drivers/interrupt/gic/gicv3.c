/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 20.10.23
 */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/irqctrl.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/critical.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <util/field.h>
#include <util/log.h>

#include "gicv3.h"

static int gic_irqctrl_init(void) {
	uint64_t reg;

	/* Distributor configuration */
	REG32_ORIN(GICD_CTLR,
	    GICD_CTLR_ARE_NS | GICD_CTLR_GRP1_NS | GICD_CTLR_GRP0);
	/* Redistributor configuration */
	REG32_CLEAR(GICR_WAKER, GICR_WAKER_PS);
	// while (!(REG32_LOAD(GICR_WAKER) & GICR_WAKER_CA)) {}

	/* CPU interface configuration */
	reg = icc_sre_el1_read();
	icc_sre_el1_write(reg | ICC_SRE_EN);
	reg = icc_sre_el2_read();
	icc_sre_el2_write(reg | ICC_SRE_EN);
	icc_pmr_el1_write(0xFF);
	reg = icc_ctlr_el1_read();
	icc_ctlr_el1_write(reg | 0x1);
	icc_igrpen0_el1_write(0x1);
	icc_igrpen1_el1_write(0x3);

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	unsigned int reg_nr;
	uint32_t value;

	assert(irq_nr_valid(irq));

	reg_nr = irq >> 5;
	value = 1U << (irq & 0x1f);

	/* Writing zeroes to this register has no
	 * effect, so we just write single "1" */
	REG32_STORE(GICD_ISENABLER(reg_nr), value);

	/* N-N irq model: all CPUs receive this IRQ */
	REG32_STORE(GICD_ICFGR(reg_nr), value);

	/* All CPUs do listen to this IRQ */
	reg_nr = irq >> 2;
	value = 0xff << ((irq & 0x3) << 3);
	REG32_ORIN(GICD_ITARGETSR(reg_nr), value);
}

void irqctrl_disable(unsigned int irq) {
	unsigned int reg_nr;
	uint32_t value;

	assert(irq_nr_valid(irq));

	reg_nr = irq >> 5;
	value = 1U << (irq & 0x1f);

	/* Writing zeroes to this register has no
	 * effect, so we just write single "1" */
	REG32_STORE(GICD_ICENABLER(reg_nr), value);
}

void irqctrl_force(unsigned int irq) {
}

int irqctrl_pending(unsigned int irq) {
	return 0;
}

/* Sends an EOI (end of interrupt) signal to the PICs. */
void irqctrl_eoi(unsigned int irq) {
	assert(irq_nr_valid(irq));

	icc_eoir1_el1_write(irq);
}

void interrupt_handle(void) {
	unsigned int irq;

	irq = icc_iar1_el1_read() & 0xffffff;

	assert(irq_nr_valid(irq));
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	irqctrl_disable(irq);
	irqctrl_eoi(irq);
	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		ipl_enable();

		irq_dispatch(irq);

		ipl_disable();
	}
	irqctrl_enable(irq);
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

void swi_handle(void) {
	printk("swi!\n");
}

PERIPH_MEMORY_DEFINE(gicd, GICD_BASE, 0x1000);
PERIPH_MEMORY_DEFINE(gicr, GICR_BASE, 0x2000);
