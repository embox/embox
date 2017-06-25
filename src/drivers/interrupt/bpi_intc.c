/**
 * @file
 * @brief Banana Pi interrupt controller driver
 *
 * @date 15.04.15
 * @author Dmitrii Petukhov
 */

#include <assert.h>

#include <kernel/critical.h>
#include <hal/reg.h>
#include <hal/ipl.h>
#include <drivers/irqctrl.h>

#include <kernel/irq.h>
#include <embox/unit.h>
#include <kernel/printk.h>

EMBOX_UNIT_INIT(bpi_intc_init);

/** On BananaPi SGI source numbers lie between 0 and 15 */
#define SGI_MAX_SRC         15

#define GIC_BASE            0x01C80000

/** 0x1000-0x1FFF Distributor */
#define GICD_BASE          (GIC_BASE + 0x1000)

/** 0x2000-0x3FFF CPU interfaces */
#define GICC_BASE          (GIC_BASE + 0x2000)

#define GICD_CTRL          (GICD_BASE + 0x000)
#define GICD_ISENABLER(n)  (GICD_BASE + 0x100 + (n) * 0x4)
#define GICD_ICENABLER(n)  (GICD_BASE + 0x180 + (n) * 0x4)
#define GICD_ISACTIVER(n)  (GICD_BASE + 0x300 + (n) * 0x4)
#define GICD_ICACTIVER(n)  (GICD_BASE + 0x380 + (n) * 0x4)
#define GICD_IPRIORITYR(n) (GICD_BASE + 0x400 + (n) * 0x4)
#define GICD_SGIR          (GICD_BASE + 0xF00)
#define GICD_CPENDSGIR(n)  (GICD_BASE + 0xF10 + (n) * 0x4)

#define GICC_CTRL          (GICC_BASE + 0x00)
#define GICC_PMR           (GICC_BASE + 0x04)
#define GICC_INTACK        (GICC_BASE + 0x0c)
#define GICC_EOI           (GICC_BASE + 0x10)

/** CPUTargetList[0] corresponds to CPU interface 0 */
#define CPU_TARGET_LIST_0 (1 << 16) 

void software_init_hook(void) {
	int i;

	REG_STORE(GICD_CTRL, 0);
	REG_STORE(GICD_CTRL, 1);

	REG_STORE(GICD_ICENABLER(0), 0xffff0000);
	REG_STORE(GICD_ISENABLER(0), 0x0000ffff);

	/**
	 * Set priority on PPI and SGI interrupts
	 */
	for (i = 0; i < 32; i += 4) {
		REG_STORE(GICD_IPRIORITYR(i >> 2), 0xa0a0a0a0);
	}

	/**
	 * The GICC_PMR for a CPU interface defines a priority threshold for the target processor.
	 * The GIC only signals pending interrupts with a higher priority than this threshold value 
	 * to the target processor.
	 */
	REG_STORE(GICC_PMR, 0xf0);

	REG_STORE(GICC_CTRL, 0x1);
}

static int bpi_intc_init(void) {
	return 0;
}

void irqctrl_enable(unsigned int interrupt_nr) {
	assert(interrupt_nr <= SGI_MAX_SRC);
	/**
	 * Writes to bits corresponding to the SGIs are ignored.
	 * SGIs are always enabled.
	 */
}

void irqctrl_disable(unsigned int interrupt_nr) {
	assert(interrupt_nr <= SGI_MAX_SRC);
	/** 
	 * Writing 1 to a Clear-enable bit disables forwarding of 
	 * the corresponding interrupt from the Distributor to the CPU interfaces
	 * Writes to bits corresponding to the SGIs are ignored.
	 * SGIs are always enabled.
	 *
	 * REG_STORE(GICD_ICENABLER(), ABC)
	 */

	/**
	 * Writing to a Clear-active bit Deactivates the corresponding interrupt
	 *
	 * REG_STORE(GICD_ICACTIVER(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
	 */
}

void irqctrl_clear(unsigned int interrupt_nr) {
	assert(interrupt_nr <= SGI_MAX_SRC);
}

void irqctrl_force(unsigned int interrupt_nr) {
	assert(interrupt_nr <= SGI_MAX_SRC);

	/** SGI is made pending by writing to the SGIR */
	REG_STORE(GICD_SGIR, CPU_TARGET_LIST_0 | (interrupt_nr & 0xf));

	/** 
	 * Writing to a Set-active bit Activates the corresponding interrupt
	 * REG_STORE(GICD_ISACTIVER(interrupt_nr >> 5), 1 << (interrupt_nr & 0x1f));
	 */
}

void interrupt_handle(void) {
	/** Read acts as an acknowledge for the interrupt */
	unsigned int irqstat = REG_LOAD(GICC_INTACK);

	unsigned int irq = irqstat & 0x3ff;

	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		ipl_enable();

		/**
		 * A processor writes to this register to inform the CPU interface either:
		 *	1. that it has completed the processing of the specified interrupt
		 *	2. in a GICv2 implementation, when the appropriate GICC_CTLR.EOImode bit 
		 *	   is set to 1, to indicate that the interface should perform priority drop 
		 *	   for the specified interrupt.
		 */
		REG_STORE(GICC_EOI, irqstat);

		irq_dispatch(irq);

		ipl_disable();

	}

	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

void swi_handle(void) {
	printk("swi!\n");
}
