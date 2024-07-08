/**
 * @file
 * @brief
 *
 * @date 21.12.2012
 * @author Anton Bulychev
 */

#ifndef ARCH_X86_AP_H_
#define ARCH_X86_AP_H_

#include <asm/gdt.h>

#ifndef __ASSEMBLER__

extern char __ap_trampoline;
extern char __ap_trampoline_end;

extern void *__ap_sp[];
extern gdt_pointer_t __ap_gdtr;
extern gdt_gate_t __ap_gdt[GDT_ENTRIES];

#endif /* __ASSEMBLER__ */

#endif /* ARCH_X86_AP_H_ */
