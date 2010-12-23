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
        uint8_t  limit_high:4;
        uint8_t  granularity:4;
        uint8_t  base_high;
} gdt_gate_t __attribute__((packed));

typedef struct gdt_pointer {
	uint16_t limit;
	uint32_t base;
} gdt_pointer_t __attribute__((packed));

#define SET_GDT(gdt_ptr)                 \
        __asm__ __volatile__(            \
                "lgdt %0" :              \
                : "m"((gdt_ptr)->limit), \
                "m"(*gdt_ptr)            \
        )

extern gdt_gate_t _gdt[];
extern gdt_pointer_t gdt_ptr;

void gdt_set_gate(uint8_t nr, uint32_t base, uint32_t limit, uint8_t ac, uint8_t gran) {
	if (limit > 0xfffff) {
		limit >>= 12;
		gran |= 0x8;
	}
	_gdt[nr].limit_low   = limit & 0xffff;
	_gdt[nr].base_low    = base & 0xffff;
	_gdt[nr].base_med    = (base >> 16) & 0xff;
	_gdt[nr].access      = ac | 0x80;
	_gdt[nr].limit_high  = limit >> 16;
	_gdt[nr].granularity = gran;
	_gdt[nr].base_high   = base >> 24;
}

void gdt_init(void) {
	gdt_set_gate(0, 0, 0, 0, 0); /* NULL Descriptor */
//	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* Code segment */
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x1A, 0x4); /* Data segment */
//	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* Code segment */
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0x12, 0x4); /* Data segment */
//	gdt_set_gate(6, 0, 0xFFFF, 0x9A, 0x00); /* 16-bit Code segment */
//	gdt_set_gate(7, 0, 0xFFFF, 0x92, 0x00); /* 16-bit Data segment */
	SET_GDT(&gdt_ptr);
}

