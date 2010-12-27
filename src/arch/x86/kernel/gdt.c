/**
 * @file
 * @brief Global Descriptor Table (GDT)
 * @details The IDT is used by the processor to determine
 *          used to define the characteristics of the various
 *          memory areas used during program execution.
 * @date 22.12.10
 * @author Nikolay Korotky
 */

#include <types.h>
#include <asm/regs.h>
#include <asm/traps.h>
#include <kernel/panic.h>

#define GDT_SIZE 8

typedef struct gdt_gate {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t  base_med;
        uint8_t  access;
 //       uint8_t  limit_high:4;
 //       uint8_t  granularity:4;
        uint8_t  granularity;
        uint8_t  base_high;
} __attribute__((packed)) gdt_gate_t;

typedef struct gdt_pointer {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) gdt_pointer_t;

#define SET_GDT(gdt_ptr)                 \
        __asm__ __volatile__(            \
                "lgdt %0" :              \
                : "m"((gdt_ptr)->limit), \
                "m"(*gdt_ptr)            \
        )
#if 0
extern gdt_gate_t _gdt[];
extern gdt_pointer_t gdt_ptr;
#endif

extern void gdt_flush(uint32_t gdt);
gdt_gate_t gdt_entries[5];
gdt_pointer_t gdt_ptr;

void gdt_set_gate(uint8_t nr, uint32_t base, uint32_t limit, uint8_t ac, uint8_t gran) {
//	if (limit > 0xfffff) {
//		limit >>= 12;
//		gran |= 0x8;
//	}

	gdt_entries[nr].base_low    = base & 0xffff;
	gdt_entries[nr].base_med    = (base >> 16) & 0xff;
	gdt_entries[nr].base_high   = (base >> 24) & 0xff;

	gdt_entries[nr].limit_low   = limit & 0xffff;
	gdt_entries[nr].granularity = (limit >> 16) & 0x0F;

	gdt_entries[nr].granularity|= (gran & 0xF0);
	gdt_entries[nr].access      = ac;
	//gdt_entries[nr].limit_high  = limit >> 16;
}

void gdt_init(void) {
	gdt_ptr.limit = sizeof(gdt_entries) - 1;
	gdt_ptr.base = (uint32_t)gdt_entries;

	gdt_set_gate(0, 0, 0, 0, 0); /* NULL Descriptor */
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* Code segment */
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* Data segment */
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* User Code segment */
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* User Data segment */

	gdt_flush((uint32_t)&gdt_ptr);
}

