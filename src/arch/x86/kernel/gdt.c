/**
 * @file
 * @brief Global Descriptor Table (GDT)
 * @details The IDT is used by the processor to determine
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

void gdt_set_gate(uint8_t nr, uint32_t base, uint32_t limit, uint8_t ac, uint8_t gran) {
	gdt_entries[nr].base_low    = base & 0xffff;
	gdt_entries[nr].base_med    = (base >> 16) & 0xff;
	gdt_entries[nr].base_high   = (base >> 24) & 0xff;

	gdt_entries[nr].limit_low   = limit & 0xffff;
	gdt_entries[nr].granularity = (limit >> 16) & 0x0F;

	gdt_entries[nr].granularity|= (gran & 0xF0);
	gdt_entries[nr].access      = ac;
}

void gdt_init(void) {
	gdt_ptr.limit = sizeof(gdt_entries) - 1;
	gdt_ptr.base = (uint32_t)gdt_entries;

	gdt_set_gate(0, 0, 0, 0, 0); /* NULL Descriptor */
	gdt_set_gate(GDT_ENTRY_KERNEL_CS, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set_gate(GDT_ENTRY_KERNEL_DS, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt_set_gate(GDT_ENTRY_USER_CS, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt_set_gate(GDT_ENTRY_USER_DS, 0, 0xFFFFFFFF, 0xF2, 0xCF);

	gdt_flush((uint32_t)&gdt_ptr);
}
