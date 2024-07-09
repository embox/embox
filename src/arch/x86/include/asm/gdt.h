/**
 * @file
 * @brief Define Interface to the CPU's global descriptor table
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#ifndef ARCH_X86_GDT_H_
#define ARCH_X86_GDT_H_

#define GDT_ENTRIES            6

#define GDT_ENTRY_SIZE        64

#define GDT_ENTRY_KERNEL_CS    1
#define GDT_ENTRY_KERNEL_DS    2
#define GDT_ENTRY_USER_CS      3
#define GDT_ENTRY_USER_DS      4
#define GDT_ENTRY_TSS          5

#define __KERNEL_CS            (GDT_ENTRY_KERNEL_CS * 8)
#define __KERNEL_DS            (GDT_ENTRY_KERNEL_DS * 8)
#define __USER_CS              (GDT_ENTRY_USER_CS * 8 + 3)
#define __USER_DS              (GDT_ENTRY_USER_DS * 8 + 3)
#define __TSS                  (GDT_ENTRY_TSS * 8 + 3)

#define GDT_BUSY_FLAG          (1 << 7)

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef struct gdt_gate {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t  base_med;
        uint8_t  access;
        uint8_t  granularity;
        uint8_t  base_high;
} __attribute__((packed)) gdt_gate_t;

typedef struct gdt_pointer {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) gdt_pointer_t;

extern gdt_gate_t gdt[GDT_ENTRIES];

extern void gdt_set_gdtr(gdt_pointer_t *gdtr, gdt_gate_t *gdt);
extern void gdt_set_gate(uint8_t nr, uint32_t base, uint32_t limit,
		uint8_t ac, uint8_t gran);
extern void gdt_flush(gdt_pointer_t *gdt);

#endif /* __ASSEMBLER__ */

#endif /* ARCH_X86_GDT_H_ */
