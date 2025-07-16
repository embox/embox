/**
 * @file
 * @brief Interrupt Descriptor Table (IDT)
 * @details The IDT is used by the processor to determine
 *          the correct response to interrupts and exceptions.
 *
 * @date 22.12.10
 * @author Nikolay Korotky
 */

#include <string.h>
#include <stdint.h>

#include <asm/io.h>
#include <asm/regs.h>
#include <asm/traps.h>
#include <asm/gdt.h>

#include <hal/cpu.h>
#include <kernel/panic.h>

#include <module/embox/arch/interrupt.h>
#include <module/embox/arch/syscall.h>

#define IDT_SIZE 256

/**
 * IDT entry, Interrupt Gates
 */
struct idt_gate {
	/**
	 * Lower part of the interrupt function's offset address (pointer).
	 */
	uint16_t offset_low;
	/**
	 * 15     3  2   1   0
	 * +-------+----+-----+
	 * | Index | TI | RPL |
	 * +-------+----+-----+
	 * RPL   - Requested Privilege Level.
	 * TI    - Table Index. 0 = GDT, 1 = LDT, table to use.
	 * Index - Index to a Descriptor of a table.
	 */
	uint16_t selector;
	/** unused, set to 0 */
	uint8_t  _zero;
	/**
	 *   7   6   5   4   3           0
	 * +---+---+---+---+---+---+---+---+
	 * | P | DPL   | S |    GateType   |
	 * +---+---+---+---+---+---+---+---+
	 * GateType:
	 * 	0x5 32 bit Task gate
	 * 	0x6 16-bit interrupt gate
	 * 	0x7 16-bit trap gate
	 * 	0xE 32-bit interrupt gate
	 * 	0xF 32-bit trap gate
	 * S   - Storage Segment
	 * DPL - Descriptor Privilege Level
	 * P   - Present, can be set to 0 for unused interrupts or for Paging.
	 */
	uint8_t  attr;
	/**
	 * Higher part of the offset.
	 */
	uint16_t offset_high;
} __attribute__((packed));

struct idt_pointer {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

static struct idt_gate idt[IDT_SIZE];

void idt_set_gate(uint8_t nr, uint32_t base, uint16_t sel, uint8_t attr) {
	idt[nr].offset_low  = base & 0xFFFF;
	idt[nr].offset_high = (base >> 16) & 0xFFFF;
	idt[nr]._zero       = 0;
	idt[nr].selector    = sel;
	idt[nr].attr        = attr;
}

#define __FWD_DECL(sym) ({ extern void sym(void); &sym; })

#define IDT_EXCEPT(nr) \
	idt_set_gate(nr, (unsigned) __FWD_DECL(t_excep##nr), __KERNEL_CS, 0x8E)

static void idt_init_except(void) {
	IDT_EXCEPT(0);  IDT_EXCEPT(1);  IDT_EXCEPT(2);  IDT_EXCEPT(3);
	IDT_EXCEPT(4);  IDT_EXCEPT(5);  IDT_EXCEPT(6);  IDT_EXCEPT(7);
	IDT_EXCEPT(8);  IDT_EXCEPT(9);  IDT_EXCEPT(10); IDT_EXCEPT(11);
	IDT_EXCEPT(12); IDT_EXCEPT(13); IDT_EXCEPT(14); IDT_EXCEPT(15);
	IDT_EXCEPT(16); IDT_EXCEPT(17); IDT_EXCEPT(18); IDT_EXCEPT(19);
	IDT_EXCEPT(20); IDT_EXCEPT(21); IDT_EXCEPT(22); IDT_EXCEPT(23);
	IDT_EXCEPT(24); IDT_EXCEPT(25); IDT_EXCEPT(26); IDT_EXCEPT(27);
	IDT_EXCEPT(28); IDT_EXCEPT(29); IDT_EXCEPT(30); IDT_EXCEPT(31);
}

#ifndef INTERRUPT_STUB

#define IDT_IRQ(nr) \
	idt_set_gate(nr + 32, (unsigned) __FWD_DECL(irq##nr), __KERNEL_CS, 0x8E)

static void idt_init_irq(void) {
	/* Master PIC */
	IDT_IRQ(0);  IDT_IRQ(1);  IDT_IRQ(2);  IDT_IRQ(3);
	IDT_IRQ(4);  IDT_IRQ(5);  IDT_IRQ(6);  IDT_IRQ(7);
	/* Slave PIC */
	IDT_IRQ(8);  IDT_IRQ(9);  IDT_IRQ(10); IDT_IRQ(11);
	IDT_IRQ(12); IDT_IRQ(13); IDT_IRQ(14); IDT_IRQ(15);
}

#else
#define idt_init_irq() do { } while(0)
#endif /* INTERRUPT_STUB */

#ifndef SYSCALL_STUB

static void idt_init_syscall(void) {
	idt_set_gate(0x80, (unsigned) __FWD_DECL(syscall_trap), __KERNEL_CS, 0xEF);
}

#else
#define idt_init_syscall() do { } while(0)
#endif

#ifdef SMP

static void idt_init_smp(void) {
	idt_set_gate(0x50, (unsigned) __FWD_DECL(resched_trap), __KERNEL_CS, 0x8E);
}

#else /* !SMP */

#define idt_init_smp() do { } while(0)

#endif /* SMP */

static struct idt_pointer idt_ptr;

void idt_load(void) {
	__asm__ __volatile__(
		"lidt %0\n\t"
		:
		: "m"(idt_ptr.limit),
		  "m"(idt_ptr)
	);
}

void idt_init(void) {
	idt_ptr.limit = sizeof(idt) - 1;
	idt_ptr.base = (uint32_t) idt;

	/* zero IDT */
	memset((unsigned char*) &idt, 0, sizeof(idt));

	idt_init_except();
	idt_init_irq();
	idt_init_syscall();
	idt_init_smp();

	idt_load();
}
