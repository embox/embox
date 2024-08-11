/**
 * @file
 * @brief
 *
 * @date 05.02.2013
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <stdint.h>

#include <hal/cpu.h>

#include <asm/io.h>

#include "lapic.h"

#include <drivers/irqctrl.h>
#include <drivers/common/memory.h>

#include <module/embox/driver/interrupt/lapic.h>

#ifdef LAPIC_REGS_X86_H_
#include "drivers/i8259_regs.h"
#endif

EMBOX_UNIT_INIT(ioapic_enable);

static inline void i8259_disable(void)
{
#ifdef IOAPIC_REGS_X86_H_ /* Needed only for x86 */
	outb(0xFF, PIC2_DATA);
	outb(0xFF, PIC1_DATA);
	//inb(PIC1_DATA);
#endif /* IOAPIC_REGS_X86_H_ */
}

/**
 * Initialize the IOAPIC
 */
static int ioapic_enable(void) {
	static int inited = 0;
	if (inited & 0x1 << cpu_get_id()) {
		return 0;
	}else
		inited |= 0x1 << cpu_get_id();

#ifdef IOAPIC_REGS_X86_H_
	/* I'm not sure that it is correct */
	outb(0x70, 0x22);
	outb(0x01, 0x23);
#endif

	return 0;
}

void apic_init(void) {
	i8259_disable();
	ioapic_enable();

	lapic_enable();
}

static inline uint32_t irq_redir_low(unsigned int irq) {
	uint32_t val = 0;

	if (irq < 16) {
		/* ISA active-high */
		val |= IOAPIC_ICR_HIGH_POLARITY;
		/* ISA edge triggered */
		val |= IOAPIC_ICR_EDGE_TRIGGER;
	}
	else {
		/* PCI active-low */
		val |= IOAPIC_ICR_LOW_POLARITY;
		/* PCI level triggered */
		val |= IOAPIC_ICR_LEVEL_TRIGGER;
	}

	val |= IOAPIC_ICR_DM_FIXED;
	val |= IOAPIC_ICR_LOGICAL_DEST;
	val |= (irq + 0x20);

	/* Return with Mask bit clear */
	return val;
}

void irqctrl_enable(unsigned int irq) {
	uint32_t low;
	static uint32_t high = 0;

	if (irq >= 24) {
		/* msi irq */
		return;
	}
	low = irq_redir_low(irq);
	/**
	 * In both SMP and NOSMP situation, we can use logical destination
	 * mode to delivery interrupt. But when we add CPU in such mode,
	 * we only need to call irqctrl_enable() at each CPUs to set bit
	 */
	high |= lapic_read(LAPIC_LDR);

	ioapic_write(IOAPIC_REDIR_TABLE + irq * 2, low);
	ioapic_write(IOAPIC_REDIR_TABLE + irq * 2 + 1, high);
}

void irqctrl_disable(unsigned int irq) {
	uint32_t low;

	if (irq == 0) {
		/*None Maskable interrupt*/
		return;
	}

	if (irq >= 24) {
		/* msi irq */
		return;
	}

	low = ioapic_read(IOAPIC_REDIR_TABLE + irq * 2);
	low |= IOAPIC_ICR_INT_MASK;
	ioapic_write(IOAPIC_REDIR_TABLE + irq * 2, low);
}

void irqctrl_force(unsigned int irq) {
	// TODO Emm?.. -- Eldar
}

int irqctrl_pending(unsigned int irq) {
	// TODO Emm?.. -- Anton
	return 1;
}

void irqctrl_eoi(unsigned int irq) {
	//TODO: irq >= 16
	if (irq >= 24) {
		/* msi irq */
		return;
	}

	lapic_send_eoi();
}

IRQCTRL_DEF(iopic, NULL);

PERIPH_MEMORY_DEFINE(iopic, IOAPIC_DEF_ADDR, 0x100);
