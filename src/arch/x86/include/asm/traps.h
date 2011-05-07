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
	uint32_t edi, esi, ebp, cr2, ebx, edx, ecx, eax; /*pusha*/
	uint32_t trapno, err;              /*push by isr*/
	uint32_t eip, cs, eflags, esp, ss; /*Pushed by the processor automatically*/
} pt_regs_t;

extern void idt_set_gate(uint8_t nr, uint32_t base, uint16_t sel, uint8_t attr);
extern void idt_init(void);
extern void gdt_init(void);

#endif /* X86_TRAPS_H_ */
