/**
 * @file
 * @brief Interrupt controller driver for i8952 chip 9x86 platform.
 *
 * @details
 *   This driver believes that there are two i8952 chip in the system and
 *   slave is connected to master's second line.
 *   We also suppose that we use only x86 platform.
 *
 * @date 22.12.10
 * @author Nikolay Korotky
 */

#include <asm/io.h>
#include <asm/regs.h>
#include <asm/traps.h>
#include <drivers/i8259.h>
#include <drivers/irqctrl.h>
#include <hal/arch.h>
#include <hal/reg.h>

#include <embox/unit.h>

#include "i8259_regs.h"

EMBOX_UNIT_INIT(unit_init);

/**
 * Initialize the PIC
 */
static int unit_init(void) {
	static int inited = 0;
	if (1 == inited) {
		return 0;
	}
	inited = 1;

	/* Initialize the master */
	out8(PIC1_ICW1, PIC1_COMMAND);
	out8(PIC1_BASE, PIC1_DATA);
	out8(PIC1_ICW3, PIC1_DATA);
	out8(PIC1_ICW4, PIC1_DATA);

	/* Initialize the slave */
	out8(PIC2_ICW1, PIC2_COMMAND);
	out8(PIC2_BASE, PIC2_DATA);
	out8(PIC2_ICW3, PIC2_DATA);
	out8(PIC2_ICW4, PIC2_DATA);

	out8(NON_SPEC_EOI, PIC1_COMMAND);
	out8(NON_SPEC_EOI, PIC2_COMMAND);

	out8(PICM_MASK, PIC1_DATA);
	out8(PICS_MASK, PIC2_DATA);

	irqctrl_enable(2); /* enable slave irq controller irq 8-16 */

	return 0;
}

void apic_init(void) {
	unit_init();
}

void irqctrl_enable(unsigned int irq) {
	if (irq < 8) {
		out8(in8(PIC1_DATA) & ~(1 << irq), PIC1_DATA);
	} else {
		out8(in8(PIC2_DATA) & ~(1 << (irq - 8)), PIC2_DATA);
	}
}

void irqctrl_disable(unsigned int irq) {
	if (irq < 8) {
		out8(in8(PIC1_DATA) | (1 << irq), PIC1_DATA);
	} else {
		out8(in8(PIC2_DATA) | (1 << (irq - 8)), PIC2_DATA);
	}
}

void irqctrl_force(unsigned int irq) {
	// TODO Emm?.. -- Eldar
}

int i8259_irq_pending(unsigned int irq) {
	if (irq < 8) {
		return in8(PIC1_COMMAND) & (1 << irq);
	} else {
		return in8(PIC2_COMMAND) & (1 << (irq - 8));
	}
}

/* Sends an EOI (end of interrupt) signal to the PICs. */
void i8259_send_eoi(unsigned int irq) {
	if (irq >= 8) {
		/* Send reset signal to slave. */
		out8(NON_SPEC_EOI, PIC2_COMMAND);
	}
	/* Send reset signal to master. (As well as to slave, if necessary). */
	out8(NON_SPEC_EOI, PIC1_COMMAND);
}
