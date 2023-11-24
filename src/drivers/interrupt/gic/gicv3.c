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

enum gic_irq_t {
	GIC_SGI,
	GIC_PPI,
	GIC_SPI,
	GIC_INVALID_TYPE,
};

static void gic_wait_for_rwp(uintptr_t reg32, uint32_t rwp_mask) {
	volatile unsigned long delay;
	volatile unsigned long i;

	delay = 100000;

	while (REG32_LOAD(reg32) & rwp_mask) {
		i = 0;
		while (i < delay) {
			i++;
			__asm__ __volatile__("nop");
		}
	}
}

static enum gic_irq_t gic_irq_type(unsigned int irq_nr) {
	switch (irq_nr) {
	case 0 ... 15:
		return GIC_SGI;
	case 16 ... 31:
		return GIC_PPI;
	case 32 ... 1019:
		return GIC_SPI;
	default:
		return GIC_INVALID_TYPE;
	}
}

static void gic_dist_init(void) {
	uint64_t mpidr;
	size_t itlines;
	uint32_t affinity;
	int i, j;

	itlines = FIELD_GET(GICD_TYPER, GICD_TYPER_ITLINES);

	/* Disable the distributor */
	REG32_STORE(GICD_CTLR, 0);
	gic_wait_for_rwp(GICD_CTLR, GICD_CTLR_RWP);

	for (i = 1; i <= itlines; i++) {
		/* Configure SPIs as non-secure Group-1 */
		REG32_STORE(GICD_IGROUPR(i), ~(uint32_t)0);

		/* Set SPIs to be level triggered */
		for (j = 0; j < 2; j++) {
			REG32_STORE(GICD_ICFGR(2 * i + j), 0);
		}

		/* Set priority for SPIs */
		for (j = 0; j < 8; j++) {
			REG32_STORE(GICD_IPRIORITYR(8 * i + j), 0xa0a0a0a0);
		}
	}

	for (i = 0; i <= itlines; i++) {
		/* Deactivate and disable SGIs/PPIs/SPIs */
		REG32_STORE(GICD_ICACTIVER(i), ~(uint32_t)0);
		REG32_STORE(GICD_ICENABLER(i), ~(uint32_t)0);
	}

	/* Enable distributor */
	REG32_STORE(GICD_CTLR,
	    GICD_CTLR_GRP0 | GICD_CTLR_GRP1_NS | GICD_CTLR_ARE_NS);
	gic_wait_for_rwp(GICD_CTLR, GICD_CTLR_RWP);

	/* Set SPIs to current CPU only */
	__asm__ __volatile__("mrs %0, MPIDR_EL1" : "=r"(mpidr));
	mpidr &= 0xff00ffffffU;
	affinity = (mpidr & 0xffffff) | ((mpidr >> 8) & 0xff000000);
	for (i = 0; i <= itlines; i++) {
		for (j = 0; j < 32; j++) {
			REG64_STORE(GICD_IROUTER(32 * i + j), affinity);
		}
	}
}

static void gic_redist_init(void) {
	int i;

	/* Make sure this CPU redistributor is quiescen */
	REG32_ORIN(GICR_WAKER, GICR_WAKER_PS);
	while (!(REG32_LOAD(GICR_WAKER) & GICR_WAKER_CA)) {}

	/* Wake up this CPU redistributor */
	REG32_CLEAR(GICR_WAKER, GICR_WAKER_PS);
	while (REG32_LOAD(GICR_WAKER) & GICR_WAKER_CA) {}

	/* Configure SGIs/PPIs as non-secure Group-1 */
	REG32_STORE(GICR_IGROUPR0, ~(uint32_t)0);

	/* Set SGIs/PPIs to be level triggered */
	for (i = 0; i < 2; i++) {
		REG32_STORE(GICR_ICFGR0, 0);
	}

	/* Set priority for SGIs/PPIs */
	for (i = 0; i < 8; i++) {
		REG32_STORE(GICR_IPRIORITYR(i), 0xa0a0a0a0);
	}

	/* Deactivate and disable SPIs */
	REG32_STORE(GICR_ICACTIVER0, ~(uint32_t)0);
	REG32_STORE(GICR_ICENABLER0, ~(uint32_t)0);

	gic_wait_for_rwp(GICR_CTLR, GICR_CTLR_RWP);
}

static void gic_cpu_init(void) {
	uint64_t reg;

	/* CPU interface configuration */
	reg = icc_sre_el1_read();
	if (!(reg & ICC_SRE_EN)) {
		icc_sre_el1_write(reg | ICC_SRE_EN);
	}

	icc_pmr_el1_write(0xff);
	icc_ctlr_el1_write(0x0);
	icc_igrpen1_el1_write(0x1);
}

static int gic_irqctrl_init(void) {
	gic_dist_init();
	gic_redist_init();
	gic_cpu_init();

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	unsigned int reg_nr;
	uint32_t value;

	assert(irq_nr_valid(irq));

	reg_nr = irq >> 5;
	value = 1U << (irq & 0x1f);

	if (gic_irq_type(irq) == GIC_SPI) {
		REG32_STORE(GICD_ISENABLER(reg_nr), value);
		REG32_STORE(GICD_ICENABLER(reg_nr), value);
		REG64_STORE(GICD_IROUTER(irq), 0x0);
		REG32_STORE(GICD_ISENABLER(reg_nr), value);
	}
	else {
		REG32_STORE(GICR_ISENABLER0, value);
	}
}

void irqctrl_disable(unsigned int irq) {
	unsigned int reg_nr;
	uint32_t value;

	assert(irq_nr_valid(irq));

	reg_nr = irq >> 5;
	value = 1U << (irq & 0x1f);

	if (gic_irq_type(irq) == GIC_SPI) {
		REG32_STORE(GICD_ICENABLER(reg_nr), value);
	}
	else {
		REG32_STORE(GICR_ICENABLER0, value);
	}
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

IRQCTRL_DEF(gicv3, gic_irqctrl_init);

PERIPH_MEMORY_DEFINE(gicd, GICD_BASE, 0x10000);
PERIPH_MEMORY_DEFINE(gicr, GICR_BASE, 0x20000);
