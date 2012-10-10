/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#ifndef ARCH_X86_GDT_H_
#define ARCH_X86_GDT_H_

#define GDT_ENTRIES_NRS_TOTAL 5

#define GDT_ENTRY_KERNEL_CS             1
#define GDT_ENTRY_KERNEL_DS             2
#define GDT_ENTRY_USER_CS               3
#define GDT_ENTRY_USER_DS               4

#define __KERNEL_CS     (GDT_ENTRY_KERNEL_CS * 8)
#define __KERNEL_DS     (GDT_ENTRY_KERNEL_DS * 8)
#define __USER_DS       (GDT_ENTRY_USER_DS * 8 + 3)
#define __USER_CS       (GDT_ENTRY_USER_CS * 8 + 3)

#ifndef __ASSEMBLER__

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

static gdt_gate_t gdt_entries[GDT_ENTRIES_NRS_TOTAL];
static gdt_pointer_t gdt_ptr;

extern void gdt_flush(uint32_t gdt);

#endif /* __ASSEMBLER__ */

#endif /* ARCH_X86_GDT_H_ */
