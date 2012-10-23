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

#ifndef __ASSEMBLER__

#include <types.h>

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

typedef struct tss_entry_struct {
	uint32_t prev_tss; // The previous TSS - if we used hardware task switching this would form a linked list.
	uint32_t esp0;     // The stack pointer to load when we change to kernel mode.
	uint32_t ss0;      // The stack segment to load when we change to kernel mode.
	uint32_t esp1;     // Unused...
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;       // The value to load into ES when we change to kernel mode.
	uint32_t cs;       // The value to load into CS when we change to kernel mode.
	uint32_t ss;       // The value to load into SS when we change to kernel mode.
	uint32_t ds;       // The value to load into DS when we change to kernel mode.
	uint32_t fs;       // The value to load into FS when we change to kernel mode.
	uint32_t gs;       // The value to load into GS when we change to kernel mode.
	uint32_t ldt;      // Unused...
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((packed)) tss_entry_t;

extern void gdt_flush(uint32_t gdt);
extern void tss_flush(void);

#endif /* __ASSEMBLER__ */

#endif /* ARCH_X86_GDT_H_ */
