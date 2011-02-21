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
#include <drivers/apic.h>

void interrupt_init(void) {
	/* remap PICs */
	out8(ICW1_INIT + ICW1_ICW4, PIC1_COMMAND);
	out8(0x20, PIC1_DATA);
	out8(0x04, PIC1_DATA);
	out8(0x01, PIC1_DATA);
	out8(0x00, PIC1_DATA);

	out8(ICW1_INIT + ICW1_ICW4, PIC2_COMMAND);
	out8(0x28, PIC2_DATA);
	out8(0x02, PIC2_DATA);
	out8(0x01, PIC2_DATA);
	out8(0x00, PIC2_DATA);
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

