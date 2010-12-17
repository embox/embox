/**
 * @file
 *
 * @date 17.12.10
 * @author Nikolay Korotky
 */

#ifndef X86_TRAPS_H_
#define X86_TRAPS_H_

#include <types.h>

#define BASE_TSS       0x08
#define KERNEL_CS      0x10
#define KERNEL_DS      0x18
#define DBF_TSS        0x20

#define ACC_TSS        0x09
#define ACC_TSS_BUSY   0x02
#define ACC_CODE_R     0x1A
#define ACC_DATA_W     0x12
#define ACC_PL_K       0x00
#define ACC_P          0x80
#define SZ_32          0x4
#define SZ_16          0x0
#define SZ_G           0x8

#define IDT_SIZE       0x14
#define GDT_SIZE       (0x28 / 8)

typedef struct gate_init_entry {
	uint32_t  entrypoint;
	uint16_t  vector;
	uint16_t  type;
} gate_init_entry_t;

typedef struct pt_regs {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, cr2, ebx, edx, ecx, eax;
	uint32_t trapno, err;
	uint32_t eip, cs, eflags, esp, ss;
} pt_regs_t;

typedef struct x86_gate {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t  word_count;
	uint8_t  access;
	uint16_t offset_high;
} x86_gate_t __attribute__((packed));

typedef struct x86_desc {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t  base_med;
	uint8_t  access;
	uint8_t  limit_high:4;
	uint8_t  granularity:4;
	uint8_t  base_high;
} x86_desc_t __attribute__((packed));

typedef struct x86_tss {
	uint32_t back_link;
	uint32_t esp0, ss0;
	uint32_t esp1, ss1;
	uint32_t esp2, ss2;
	uint32_t cr3;
	uint32_t eip, eflags;
	uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint32_t es, cs, ss, ds, fs, gs;
	uint32_t ldt;
	uint16_t trace_trap;
	uint16_t io_bit_map_offset;
} x86_tss_t;

typedef struct ps_desc {
	uint16_t pad;
	uint16_t limit;
	uint32_t linear_base;
} ps_desc_t;

#define SET_IDT(pseudo_desc)                 \
        __asm__ __volatile__(                \
    		"lidt %0" :                  \
    		: "m"((pseudo_desc)->limit), \
    		  "m"(*pseudo_desc)          \
    	)

#define SET_GDT(pseudo_desc)                 \
	__asm__ __volatile__(                \
		"lgdt %0" :                  \
		: "m"((pseudo_desc)->limit), \
		"m"(*pseudo_desc)            \
	)

#define SET_TR(seg)                         \
	__asm__ __volatile__(               \
		"ltr %0"                    \
		: : "rm"((uint16_t) (seg)) \
	)

#endif /* X86_TRAPS_H_ */

