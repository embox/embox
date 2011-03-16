/**
 * @file
 * @brief Advanced Programmable Interrupt Controller (APIC) for x86.
 *
 * @date 22.12.10
 * @author Nikolay Korotky
 */

#include <types.h>
#include <hal/arch.h>
#include <hal/interrupt.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <hal/interrupt.h>
#include <asm/regs.h>
#include <asm/traps.h>
#include <asm/io.h>
#include <asm/cpu.h>
#include <drivers/apic.h>

/**
 * Initialize the PIC
 */
void interrupt_init(void) {
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

	apic_disable_all();
	irq_enable();
}

void interrupt_enable(interrupt_nr_t int_nr) {
	if (int_nr > 8) {
		out8(in8(PIC2_DATA) & ~(1 << int_nr), PIC2_DATA);
	} else {
		out8(in8(PIC1_DATA) & ~(1 << int_nr), PIC1_DATA);
	}
}

void interrupt_disable(interrupt_nr_t int_nr) {
	if (int_nr > 8) {
		out8(in8(PIC2_DATA) | (1 << int_nr), PIC2_DATA);
	} else {
		out8(in8(PIC1_DATA) | (1 << int_nr), PIC1_DATA);
	}
}

void interrupt_force(interrupt_nr_t irq_num) {

}

void irqc_set_mask(__interrupt_mask_t mask) {
	out8(mask & 0xff, PIC1_DATA);
	out8((mask >> 8) & 0xff, PIC2_DATA);
}

__interrupt_mask_t irqc_get_mask(void) {
	return (in8(PIC2_DATA) << 8) | in8(PIC1_DATA);
}

