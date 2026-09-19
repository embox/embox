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
#include <hal/cpu.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <kernel/critical.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <util/field.h>
#include <util/log.h>

#include "gicv1.h"

#define GIC_SPURIOUS_IRQ 0x3FF

#ifdef SMP
/* GICD_SGIR target mask of each logical CPU, 0 until it runs
 * irqctrl_init_cpu() */
static uint8_t gic_cpu_target[NCPU];
#endif /* SMP */

/* Interface that receives the SPIs: the boot CPU's, and every interface
 * until the distributor is initialised */
static uint8_t gic_boot_target = 0xff;

/* The per-CPU half: GICC_PMR and GICC_CTLR are banked per CPU interface */
void irqctrl_init_cpu(void) {
	uint32_t reg;

	/* Enable interrupts of all priorities */
	reg = REG32_LOAD(GICC_PMR);
	reg = FIELD_SET(reg, GICD_PMR_PRIOR, 0xff);
	REG32_STORE(GICC_PMR, reg);

	REG32_ORIN(GICC_CTLR, GICC_CTLR_EN);

#ifdef SMP
	/* GICD_ITARGETSR is banked for SGIs and PPIs: it reads back this CPU's
	 * own interface bit */
	gic_cpu_target[cpu_get_id()] = REG32_LOAD(GICD_ITARGETSR(0)) & 0xff;
#endif /* SMP */
}

#ifdef SMP
void irqctrl_send_ipi(unsigned int cpu_id, unsigned int irq) {
	uint32_t target;

	assert(irq < 16);

	if (cpu_id >= NCPU) {
		return;
	}
	target = gic_cpu_target[cpu_id];
	if (!target) {
		/* Not a CPU that has run irqctrl_init_cpu() */
		return;
	}

	/* What the target is woken to look at must be visible before the SGI */
	dsb(sy);
	REG32_STORE(GICD_SGIR, (target << 16) | (irq & 0xf));
}
#endif /* SMP */

static int gic_irqctrl_init(void) {
	uint32_t reg;

	irqctrl_init_cpu();

	/* This CPU owns the shared interrupts from here on. */
	gic_boot_target = REG32_LOAD(GICD_ITARGETSR(0)) & 0xff;

	/* Configure control registers */
	REG32_ORIN(GICD_CTLR, GICD_CTLR_EN);

	/* Print info */
	reg = REG32_LOAD(GICD_TYPER);

	log_info("Number of SPI: %zi",
	    (size_t)(FIELD_GET(reg, GICD_TYPER_ITLINES) * 32));
	log_info("Number of supported CPU interfaces: %zi",
	    (size_t)FIELD_GET(reg, GICD_TYPER_CPU));

	if (reg & GICD_TYPER_SECEXT) {
		log_info("Secutity Extension implemented");
		log_info("Number of LSPI: %zi",
		    (size_t)FIELD_GET(reg, GICD_TYPER_LSPI));
	}
	else {
		log_info("Secutity Extension not implemented");
		log_info("LSPI not implemented");
	}

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	unsigned int reg_nr;
	uint32_t value;
	uint32_t shift;

	assert(irq_nr_valid(irq));

	reg_nr = irq >> 5;
	value = 1U << (irq & 0x1f);

	/* Writing zeroes to this register has no
	 * effect, so we just write single "1" */
	REG32_STORE(GICD_ISENABLER(reg_nr), value);

	/* N-N irq model: all CPUs receive this IRQ */
	REG32_STORE(GICD_ICFGR(reg_nr), value);

	/* SPIs go to the boot CPU only, as GICv3 does through GICD_IROUTER.
	 * ITARGETSR is read-only for SGIs and PPIs, which are per-CPU anyway. */
	if (irq >= 32) {
		reg_nr = irq >> 2;
		shift = (irq & 0x3) << 3;
		value = REG32_LOAD(GICD_ITARGETSR(reg_nr));
		value &= ~(0xffU << shift);
		value |= (uint32_t)gic_boot_target << shift;
		REG32_STORE(GICD_ITARGETSR(reg_nr), value);
	}
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

/* The raw GICC_IAR, which GICC_EOIR wants back: an SGI carries the sending
 * CPU in bits [12:10]. One slot per CPU, because handlers do not nest. */
#define GICC_IAR_INTID_MASK 0x3ffU

#ifdef SMP
static uint32_t gic_last_iar[NCPU];
#define GIC_LAST_IAR gic_last_iar[cpu_get_id()]
#else
static uint32_t gic_last_iar;
#define GIC_LAST_IAR gic_last_iar
#endif /* SMP */

/* Sends an EOI (end of interrupt) signal to the PICs. */
void irqctrl_eoi(unsigned int irq) {
	assert(irq_nr_valid(irq));

	REG32_STORE(GICC_EOIR,
	    (GIC_LAST_IAR & ~GICC_IAR_INTID_MASK) | (irq & GICC_IAR_INTID_MASK));
}

int irqctrl_get_intid(void) {
	uint32_t iar;

	iar = REG32_LOAD(GICC_IAR);
	GIC_LAST_IAR = iar;

	if ((iar & GICC_IAR_INTID_MASK) == GIC_SPURIOUS_IRQ) {
		return -1;
	}

	return (int)(iar & GICC_IAR_INTID_MASK);
}

IRQCTRL_DEF(gicv1, gic_irqctrl_init);

PERIPH_MEMORY_DEFINE(gicd, GICD_BASE, 0x1000);
PERIPH_MEMORY_DEFINE(gicc, GICC_BASE, 0x2020);
