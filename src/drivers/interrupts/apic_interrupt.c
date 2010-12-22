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
	out8(0x20, 0x11);
	out8(0xA0, 0x11);
	out8(0x21, 0x20);
	out8(0xA1, 0x28);
	out8(0x21, 0x04);
	out8(0xA1, 0x02);
	out8(0x21, 0x01);
	out8(0xA1, 0x01);
	out8(0x21, 0x0);
	out8(0xA1, 0x0);
	/* Master PIC */
	idt_set_gate(32, (unsigned) irq0,  0x08, 0x8E);
	idt_set_gate(33, (unsigned) irq1,  0x08, 0x8E);
	idt_set_gate(34, (unsigned) irq2,  0x08, 0x8E);
	idt_set_gate(35, (unsigned) irq3,  0x08, 0x8E);
	idt_set_gate(36, (unsigned) irq4,  0x08, 0x8E);
	idt_set_gate(37, (unsigned) irq5,  0x08, 0x8E);
	idt_set_gate(38, (unsigned) irq6,  0x08, 0x8E);
	idt_set_gate(39, (unsigned) irq7,  0x08, 0x8E);
	/* Slave PIC */
	idt_set_gate(40, (unsigned) irq8,  0x08, 0x8E);
	idt_set_gate(41, (unsigned) irq9,  0x08, 0x8E);
	idt_set_gate(42, (unsigned) irq10, 0x08, 0x8E);
	idt_set_gate(43, (unsigned) irq11, 0x08, 0x8E);
	idt_set_gate(44, (unsigned) irq12, 0x08, 0x8E);
	idt_set_gate(45, (unsigned) irq13, 0x08, 0x8E);
	idt_set_gate(46, (unsigned) irq14, 0x08, 0x8E);
	idt_set_gate(47, (unsigned) irq15, 0x08, 0x8E);
}

void interrupt_enable(interrupt_nr_t interrupt_nr) {

}

void irq_handler(pt_regs_t *r) {
	out8(0x20, 0x20);
}

