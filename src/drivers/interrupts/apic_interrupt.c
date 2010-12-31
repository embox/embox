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

#define PIC1            0x20   /* IO base address for master PIC */
#define PIC2            0xA0   /* IO base address for slave PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)

#define PIC_EOI         0x20   /* End-of-interrupt command code */

#define ICW1_ICW4       0x01   /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02   /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04   /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08   /* Level triggered (edge) mode */
#define ICW1_INIT       0x10   /* Initialization - required! */

#define ICW4_8086       0x01   /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02   /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08   /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C   /* Buffered mode/master */
#define ICW4_SFNM       0x10   /* Special fully nested (not) */

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

void interrupt_enable(interrupt_nr_t interrupt_nr) {
//	if (interrupt_nr > 8) {
//		out8(in8(PIC2_DATA) & ~(1 << interrupt_nr), PIC2_DATA);
//	} else {
//		out8(in8(PIC2_DATA) & ~(1 << interrupt_nr), PIC1_DATA);
//	}
}

void irq_handler(pt_regs_t regs) {
	/* Send an EOI (end of interrupt) signal to the PICs.
	   If this interrupt involved the slave. */
	//FIXME: regs->trapno?
	if (regs.trapno >= 40) {
		/* Send reset signal to slave. */
		out8(PIC2_COMMAND, PIC_EOI);
	}
	/* Send reset signal to master. (As well as slave, if necessary). */
	out8(PIC1_COMMAND, PIC_EOI);
#ifdef CONFIG_IRQ
	irq_dispatch(regs.trapno - 32);
#endif
}

