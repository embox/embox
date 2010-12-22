/**
 * @file
 *
 * @date 17.12.10
 * @author Nikolay Korotky
 */

#ifndef X86_TRAPS_H_
#define X86_TRAPS_H_

#include <types.h>

typedef struct pt_regs {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, cr2, ebx, edx, ecx, eax;
	uint32_t trapno, err;
	uint32_t eip, cs, eflags, esp, ss;
} pt_regs_t;

extern void idt_init(void);
extern void gdt_init(void);

#endif /* X86_TRAPS_H_ */

