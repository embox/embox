/**
 * @file
 * @brief Global Descriptor Table (GDT)
 * @details The GDT is used by the processor to determine
 *          used to define the characteristics of the various
 *          memory areas used during program execution.
 *
 * @date 22.12.10
 * @author Nikolay Korotky
 */

#include <types.h>
#include <asm/regs.h>
#include <asm/traps.h>
#include <asm/gdt.h>
#include <kernel/panic.h>
#include <string.h>

gdt_gate_t gdt[GDT_ENTRIES];
gdt_pointer_t gdt_ptr;
tss_entry_t tss_entry;

static inline void tss_fill(void);

void gdt_set_gate(uint8_t nr, uint32_t base, uint32_t limit, uint8_t ac, uint8_t gran) {
	gdt[nr].base_low    = base & 0xffff;
	gdt[nr].base_med    = (base >> 16) & 0xff;
	gdt[nr].base_high   = (base >> 24) & 0xff;

	gdt[nr].limit_low   = limit & 0xffff;
	gdt[nr].granularity = (limit >> 16) & 0x0F;

	gdt[nr].granularity|= (gran & 0xF0);
	gdt[nr].access      = ac;
}

void gdt_init(void) {
	gdt_ptr.limit = sizeof(gdt_gate_t) * GDT_ENTRIES - 1;
	gdt_ptr.base = (uint32_t)gdt;

	gdt_set_gate(0, 0, 0, 0, 0); /* NULL Descriptor */
	gdt_set_gate(GDT_ENTRY_KERNEL_CS, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set_gate(GDT_ENTRY_KERNEL_DS, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt_set_gate(GDT_ENTRY_USER_CS, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt_set_gate(GDT_ENTRY_USER_DS, 0, 0xFFFFFFFF, 0xF2, 0xCF);
	tss_fill();  /* XXX: This need only for usermode. */

	gdt_flush((uint32_t)&gdt_ptr);
	tss_flush(); /* XXX: This need only for usermode. */
}

static inline void tss_fill() {
	// Firstly, let's compute the base and limit of our entry into the GDT.
	uint32_t base = (uint32_t) &tss_entry;
	uint32_t limit = base + sizeof(tss_entry);

	// Now, add our TSS descriptor's address to the GDT.
	gdt_set_gate(GDT_ENTRY_TSS, base, limit, 0xE9, 0x00);

	// Ensure the descriptor is initially zero.
	memset(&tss_entry, 0, sizeof(tss_entry));

	tss_entry.ss0  = __KERNEL_DS;  // Set the kernel stack segment.
	tss_entry.esp0 = 0x25000;      // TODO: Set the kernel stack pointer.

	/*
	 * Here we set the cs, ss, ds, es, fs and gs entries in the TSS. These
	 * specify what segments should be loaded when the processor switches to
	 * kernel mode. Therefore they are just our normal kernel code/data
	 * segments, but with the last two bits set. The setting of these bits
	 * sets the RPL (requested privilege level) to 3, meaning that this TSS
	 * can be used to switch to kernel mode from ring 3.
	 */
	tss_entry.cs = __KERNEL_CS | 0x3;
	tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = __KERNEL_DS | 0x3;
}
