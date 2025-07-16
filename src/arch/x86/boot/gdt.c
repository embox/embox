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

#include <asm/gdt.h>

gdt_gate_t gdt[GDT_ENTRIES];
static gdt_pointer_t gdt_ptr;

void gdt_set_gate(uint8_t nr, uint32_t base, uint32_t limit, uint8_t ac, uint8_t gran) {
	gdt[nr].base_low    = base & 0xffff;
	gdt[nr].base_med    = (base >> 16) & 0xff;
	gdt[nr].base_high   = (base >> 24) & 0xff;

	gdt[nr].limit_low   = limit & 0xffff;
	gdt[nr].granularity = (limit >> 16) & 0x0F;

	gdt[nr].granularity|= (gran & 0xF0);
	gdt[nr].access      = ac;
}

void gdt_set_gdtr(gdt_pointer_t *gdtr, gdt_gate_t *gdt) {
	gdtr->limit = sizeof(gdt_gate_t) * GDT_ENTRIES - 1;
	gdtr->base = (uint32_t) gdt;
}

void gdt_init(void) {
	gdt_set_gdtr(&gdt_ptr, gdt);

	gdt_set_gate(0, 0, 0, 0, 0); /* NULL Descriptor */
	gdt_set_gate(GDT_ENTRY_KERNEL_CS, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set_gate(GDT_ENTRY_KERNEL_DS, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt_set_gate(GDT_ENTRY_USER_CS, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt_set_gate(GDT_ENTRY_USER_DS, 0, 0xFFFFFFFF, 0xF2, 0xCF);
	/* TSS is filled in usermode (if we have) */
	gdt[GDT_ENTRY_TSS].access &= ~GDT_BUSY_FLAG;

	gdt_flush(&gdt_ptr);
}
