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
	uint64_t reg64;
	uint32_t reg32;

#if GIC_VERSION == 3
	/* Distributor configuration */
	REG32_ORIN(GICD_CTLR,
	    GICD_CTLR_ARE_NS | GICD_CTLR_GRP1_NS | GICD_CTLR_GRP0);
	/* Redistributor configuration */
	REG32_CLEAR(GICR_WAKER, GICR_WAKER_PS);
	// while (!(REG32_LOAD(GICR_WAKER) & GICR_WAKER_CA)) {}

	/* CPU interface configuration */
#if __aarch64__
	reg64 = icc_sre_el1_read();
	icc_sre_el1_write(reg64 | ICC_SRE_EN);
	reg64 = icc_sre_el2_read();
	icc_sre_el2_write(reg64 | ICC_SRE_EN);
	icc_pmr_el1_write(0xFF);
	reg64 = icc_ctlr_el1_read();
	icc_ctlr_el1_write(reg64 | 0x1);
	icc_igrpen0_el1_write(0x1);
	icc_igrpen1_el1_write(0x3);
#else  /* __aarch64__ */
/* TODO: aarch32 support */
#endif /* __aarch64__ */
#else  /* GIC_VERSION */
	/* Enable interrupts of all priorities */
	reg32 = REG32_LOAD(GICC_PMR);
	reg32 = FIELD_SET(reg32, GICD_PMR_PRIOR, 0xff);
	REG32_STORE(GICC_PMR, reg32);

	/* Configure control registers */
	REG32_ORIN(GICD_CTLR, GICD_CTLR_EN);
	REG32_ORIN(GICC_CTLR, GICC_CTLR_EN);
#endif /* GIC_VERSION */

	/* Print info if log_levle >= 3 */
	reg32 = REG32_LOAD(GICD_TYPER);

	log_info("Number of SPI: %i", (int)FIELD_GET(reg32, GICD_TYPER_ITLINES));
	log_info("Number of supported CPU interfaces: %i",
	    (int)FIELD_GET(reg32, GICD_TYPER_CPU));

	if (reg32 & GICD_TYPER_SECEXT) {
		log_info("Secutity Extension implemented");
		log_info("Number of LSPI: %i", (int)FIELD_GET(reg32, GICD_TYPER_LSPI));
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

#if GIC_VERSION == 3
#if __aarch64__
	icc_eoir1_el1_write(irq);
#else  /* __aarch64__ */
/* TODO: aarch32 support */
#endif /* __aarch64__ */
#else  /* GIC_VERSION */
	REG32_STORE(GICC_EOIR, irq);
#endif /* GIC_VERSION */
}

static int gic_irqctrl_init(void) {
	__irqctrl_init();

	return 0;
}

IRQCTRL_DEF(gic, gic_irqctrl_init);

void irqctrl_enable(unsigned int irq) {
	assert(irq_nr_valid(irq));

	__irqctrl_enable(irq);
}

void irqctrl_disable(unsigned int irq) {
	assert(irq_nr_valid(irq));

	__irqctrl_disable(irq);
}

void irqctrl_force(unsigned int irq) {
}

int irqctrl_pending(unsigned int irq) {
	return 0;
}

/* Sends an EOI (end of interrupt) signal to the PICs. */
void irqctrl_eoi(unsigned int irq) {
	assert(irq_nr_valid(irq));

	__irqctrl_eoi(irq);
}
void interrupt_handle(void) {
	unsigned int irq;

#if GIC_VERSION == 3
#if __aarch64__
	irq = icc_iar1_el1_read() & 0xffffff;
#else  /* __aarch64__ */
/* TODO: aarch32 support */
#endif /* __aarch64__ */
#else  /* GIC_VERSION */
	irq = REG32_LOAD(GICC_IAR);
	if (irq == GIC_SPURIOUS_IRQ) {
		return;
	}
#endif /* GIC_VERSION */

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

#if GIC_VERSION < 3
PERIPH_MEMORY_DEFINE(gicc, GICC_BASE, 0x2000);
#else
PERIPH_MEMORY_DEFINE(gicr, GICR_BASE, 0x2000);
#endif
