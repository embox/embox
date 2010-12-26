/**
 * @file
 * @brief Interrupt Descriptor Table (IDT)
 * @details The IDT is used by the processor to determine
 *          the correct response to interrupts and exceptions.
 *
 * @date 22.12.10
 * @author Nikolay Korotky
 */

#include <types.h>
#include <asm/regs.h>
#include <asm/traps.h>
#include <kernel/panic.h>

#define IDT_SIZE 256

/**
 * IDT entry, Interrupt Gates
 */
typedef struct idt_gate {
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
} __attribute__((packed)) idt_gate_t ;

typedef struct idt_pointer {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) idt_pointer_t ;

#define SET_IDT(idt_ptr)                  \
	__asm__ __volatile__(             \
		"lidt %0\n\t" :           \
		: "m"((idt_ptr)->limit),  \
		  "m"(*idt_ptr)           \
	)

#define IDT_ENTRY(nr) \
	extern void t_excep##nr(void);  \
	idt_set_gate(nr, (unsigned) t_excep##nr, 0x08, 0x8E)
#if 0
extern idt_gate_t _idt[];
extern idt_pointer_t idt_ptr;
#endif

idt_gate_t _idt[256];
idt_pointer_t idt_ptr;

void idt_set_gate(uint8_t nr, uint32_t base, uint16_t sel, uint8_t attr) {
	_idt[nr].offset_low  = base & 0xFFFF;
	_idt[nr].offset_high = (base >> 16) & 0xFFFF;
	_idt[nr]._zero       = 0;
	_idt[nr].selector    = sel;
	_idt[nr].attr        = attr;
}

extern void t_excep(void);


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

#include <asm/io.h>

static inline void tmp_irqen(void) {

	__asm__ __volatile__(
		"sti "
		:
		:
	);

}
void idt_init(void) {
	idt_ptr.limit = sizeof(_idt) - 1;
	idt_ptr.base = _idt;

	IDT_ENTRY(0);  IDT_ENTRY(1);  IDT_ENTRY(2);  IDT_ENTRY(3);
	IDT_ENTRY(4);  IDT_ENTRY(5);  IDT_ENTRY(6);  IDT_ENTRY(7);
	IDT_ENTRY(8);  IDT_ENTRY(9);  IDT_ENTRY(10); IDT_ENTRY(11);
	IDT_ENTRY(12); IDT_ENTRY(13); IDT_ENTRY(14); IDT_ENTRY(15);
	IDT_ENTRY(16); IDT_ENTRY(17); IDT_ENTRY(18); IDT_ENTRY(19);
	IDT_ENTRY(20); IDT_ENTRY(21); IDT_ENTRY(22); IDT_ENTRY(23);
	IDT_ENTRY(24); IDT_ENTRY(25); IDT_ENTRY(26); IDT_ENTRY(27);
	IDT_ENTRY(28); IDT_ENTRY(29); IDT_ENTRY(30); IDT_ENTRY(31);


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

	out8(0xFD,0x21);

	SET_IDT(&idt_ptr);
	tmp_irqen();
}

void exception_handler(pt_regs_t *st) {
        panic("EXECPTION:\nEAX=%08x ECX=%08x ECX=%08x EDX=%08x\n",
                st->eax, st->ebx, st->ecx, st->edx);
}

