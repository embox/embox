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
#include <asm/regs.h>
#include <asm/traps.h>
#include <asm/io.h>

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);


void interrupt_init(void) {

	/* remap PICs */
	out8(0x11, 0x20);
	out8(0x20, 0x21);
	out8(0x04, 0x21);
	out8(0x01, 0x21);
	out8(0x00, 0x21);

	out8(0x11, 0xA0);
	out8(0x28, 0xA1);
	out8(0x02, 0xA1);
	out8(0x01, 0xA1);
	out8(0x0, 0xA1);

	out8(0xFF,0x21);
	out8(0xFF,0xA1);
}

void interrupt_enable(interrupt_nr_t interrupt_nr) {
	if (interrupt_nr > 8) {
		out8(in8(0xA1) | 1 << interrupt_nr, 0xA1);
	} else {
		out8(in8(0xA1) | 1 << interrupt_nr, 0xA1);
	}
}


void irq_handler(pt_regs_t *r) {
	out8(0x20, 0x20);
}

