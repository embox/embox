/**
 * @file gicv3.c
 * @brief ARM Generic Interrupt Controller v3 driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 24.07.2019
 */

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <drivers/irqctrl.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <asm/hal/reg.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <util/log.h>

#define GIC_REDISTRIBUTOR_BASE OPTION_GET(NUMBER, redistributor_base_addr)
#define GIC_DISTRIBUTOR_BASE   OPTION_GET(NUMBER, distributor_base_addr)

EMBOX_UNIT_INIT(gic_init);

/* GIC Redistributor registers */
#define GICR_WAKER       (GIC_REDISTRIBUTOR_BASE + 0x014)
#define GICR_PROPBASER   (GIC_REDISTRIBUTOR_BASE + 0x070)
#define GICR_IGROUPR0    (GIC_REDISTRIBUTOR_BASE + 0x080)
#define GICR_ISENABLER0  (GIC_REDISTRIBUTOR_BASE + 0x100)
#define GICR_ICENABLER0  (GIC_REDISTRIBUTOR_BASE + 0x180)
#define GICR_ISPENDR0    (GIC_REDISTRIBUTOR_BASE + 0x200)
#define GICR_ICPENDR0    (GIC_REDISTRIBUTOR_BASE + 0x280)
#define GICR_ISACTIVER0  (GIC_REDISTRIBUTOR_BASE + 0x300)
#define GICR_ICACTIVER0  (GIC_REDISTRIBUTOR_BASE + 0x380)

/* GIC Distributor registers */
#define GICD_CTLR             (GIC_DISTRIBUTOR_BASE + 0x00)
#define GICD_TYPER            (GIC_DISTRIBUTOR_BASE + 0x04)
#define GICD_IIDR             (GIC_DISTRIBUTOR_BASE + 0x08)

#define GICD_TYPER_ITLINES(x) ((x) & 0x1F)
#define GICD_TYPER_CPUNR(x)   (((x) & (0x7 << 5)) >> 5)
#define GICD_TYPER_SECEXT(x)  ((x) & (1 << 10))
#define GICD_TYPER_LSPI(x)    (((x) & (0x1F << 11)) >> 11)

#define BITS_PER_REGISTER     32

#define GICD_IGROUPR(n)       (GIC_DISTRIBUTOR_BASE + 0x080 + 4 * (n))
#define GICD_ISENABLER(n)     (GIC_DISTRIBUTOR_BASE + 0x100 + 4 * (n))
#define GICD_ICENABLER(n)     (GIC_DISTRIBUTOR_BASE + 0x180 + 4 * (n))
#define GICD_ISPENDR(n)       (GIC_DISTRIBUTOR_BASE + 0x200 + 4 * (n))
#define GICD_ICPENDR(n)       (GIC_DISTRIBUTOR_BASE + 0x280 + 4 * (n))

#define GICD_ISACTIVER(n)     (GIC_DISTRIBUTOR_BASE + 0x300 + 4 * (n))
#define GICD_ICACTIVER(n)     (GIC_DISTRIBUTOR_BASE + 0x380 + 4 * (n))
#define GICD_IPRIORITYR(n)    (GIC_DISTRIBUTOR_BASE + 0x400 + 4 * (n))

#define GICD_ITARGETSR(n)     (GIC_DISTRIBUTOR_BASE + 0x800 + 4 * (n))

#define GICD_ICFGR(n)         (GIC_DISTRIBUTOR_BASE + 0xC00 + 4 * (n))
#define GICD_NSACR(n)         (GIC_DISTRIBUTOR_BASE + 0xE00 + 4 * (n))

#define GICD_SGIR             (GIC_DISTRIBUTOR_BASE + 0xF00) /* Software Generated Interrupt Register */
#define GICD_CPENDSGIR(n)     (GIC_DISTRIBUTOR_BASE + 0xF10 + 4 * (n))
#define GICD_SPENDSGIR(n)     (GIC_DISTRIBUTOR_BASE + 0xF20 + 4 * (n))

#define SPURIOUS_IRQ          0x3FF

static int gic_ctrl_init(void) {
	uint32_t tmp = REG32_LOAD(GICD_CTLR);
	uint64_t t64;

	REG32_STORE(GICD_CTLR, 0x13);

	t64 = icc_ctlr_el1_read();
	icc_ctlr_el1_write(t64 | 0x1);
	icc_igrpen0_el1_write(0x1);
	icc_igrpen1_el1_write(0x3);

	return 0;
}

static int gic_init(void) {
	/* Set priority filter level */
	icc_pmr_el1_write(0xFF);

	tmp = REG32_LOAD(GICD_TYPER);
	log_info("Number of SPI: %"PRIu32"\n", GICD_TYPER_ITLINES(tmp) * 32);
	log_info("Number of supported CPU interfaces: %"PRIu32"\n", 1 + GICD_TYPER_CPUNR(tmp));
	log_info("Secutity Extension %s implemented\n", GICD_TYPER_SECEXT(tmp) ? "" : "not ");
	if (tmp & (1 << 10)) {
		log_info("Number of LSPI: %"PRIu32, GICD_TYPER_LSPI(tmp));
	} else {
		log_info("LSPI not implemented");
	}

	REG32_STORE(GICR_WAKER, REG32_LOAD(GICR_WAKER) & ~2);
	REG32_STORE(GICR_IGROUPR0, ~0);

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	int n = irq / BITS_PER_REGISTER;
	int m = irq % BITS_PER_REGISTER;
	uint32_t tmp;

	/* Writing zeroes to this register has no
	 * effect, so we just write single "1" */
	REG32_STORE(GICD_ISENABLER(n), 1 << m);

	/* N-N irq model: all CPUs receive this IRQ */
	REG32_STORE(GICD_ICFGR(n), 1 << m);

	/* All CPUs do listen to this IRQ */
	n = irq / 4;
	m = irq % 4;
	tmp  = REG32_LOAD(GICD_ITARGETSR(n));
	tmp |= 0xFF << (8 * m);
	REG32_STORE(GICD_ITARGETSR(n), tmp);
}

void irqctrl_disable(unsigned int irq) {
	int n = irq / BITS_PER_REGISTER;
	int m = irq % BITS_PER_REGISTER;

	/* Writing zeroes to this register has no
	 * effect, so we just write single "1" */
	REG32_STORE(GICD_ICENABLER(n), 1 << m);
}

void irqctrl_force(unsigned int irq) {
}

int irqctrl_pending(unsigned int irq) {
	return 0;
}

/* Sends an EOI (end of interrupt) signal to the PICs. */
void irqctrl_eoi(unsigned int irq) {
	icc_eoir0_el1_write(irq);
}

void interrupt_handle(void) {
	unsigned int irq = icc_iar0_el1_read();

	if (irq == SPURIOUS_IRQ)
		return;

	/* TODO check if IRQ number is correct */

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

PERIPH_MEMORY_DEFINE(gic, GIC_DISTRIBUTOR_BASE, 0x2020);

IRQCTRL_DEF(gicv3, gicv3_cntl_init);
