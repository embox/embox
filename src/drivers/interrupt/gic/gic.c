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

#define GIC_VERSION OPTION_GET(NUMBER, version)

#if GIC_VERSION == 1
#include "gicv1.h"
#elif GIC_VERSION == 2
#include "gicv2.h"
#elif GIC_VERSION == 3
#include "gicv3.h"
#else
#error "Unsupported GIC version"
#endif

#define GIC_SPURIOUS_IRQ 0x3FF

static int gic_ctrl_init(void) {
	uint32_t reg;
	size_t num;

	/* Enable interrupts of all priorities */
	reg = REG32_LOAD(GICC_PMR);
	reg = FIELD_SET(reg, GICD_PMR_PRIOR, 0xff);
	REG32_STORE(GICC_PMR, reg);

	/* Configure control registers */
	REG32_ORIN(GICD_CTLR, GICD_CTLR_EN);
	REG32_ORIN(GICC_CTLR, GICC_CTLR_EN);

	/* Print info */
	reg = REG32_LOAD(GICD_TYPER);
	num = FIELD_GET(reg, GICD_TYPER_ITLINES);
	log_info("Number of SPI: %zi", num);
	num = FIELD_GET(reg, GICD_TYPER_CPU);
	log_info("Number of supported CPU interfaces: %zi", num);
	log_info("Secutity Extension %s implemented",
	    (reg & GICD_TYPER_SECEXT) ? "" : "not");
	if (reg & GICD_TYPER_SECEXT) {
		num = FIELD_GET(reg, GICD_TYPER_LSPI);
		log_info("Number of LSPI: %zi", num);
	}
	else {
		log_info("LSPI not implemented");
	}

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

	REG32_STORE(GICC_EOIR, irq);
}

void interrupt_handle(void) {
	unsigned int irq;

	irq = REG32_LOAD(GICC_IAR);
	if (irq == GIC_SPURIOUS_IRQ) {
		return;
	}

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
PERIPH_MEMORY_DEFINE(gicc, GICC_BASE, 0x2000);

#if GIC_VERSION >= 3
PERIPH_MEMORY_DEFINE(gicr, GICR_BASE, 0xc0000);
#endif

IRQCTRL_DEF(gic, gic_ctrl_init);
