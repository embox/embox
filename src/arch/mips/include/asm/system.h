/**
 * @file
 *
 *  @date May 29, 2021
 *  @author Anton Bondarev
 */

#ifndef SRC_ARCH_MIPS_INCLUDE_ASM_SYSTEM_H_
#define SRC_ARCH_MIPS_INCLUDE_ASM_SYSTEM_H_

#include <util/macro.h>

#define __ASSEMBLER__
#include <asm/asm.h>
#undef __ASSEMBLER__

static inline void execution_hazard_barrier(void) {
	__asm__ __volatile__(
		".set noreorder\n"
		"ehb\n"
		".set reorder");
}

static inline void instruction_hazard_barrier(void) {
	unsigned long tmp;

	__asm__ __volatile__(
	MACRO_STRING(PTR_LA) "\t%0, 1f\n"
	"	jr.hb	%0\n"
	"1:	.insn"
	: "=&r"(tmp));
}

#endif /* SRC_ARCH_MIPS_INCLUDE_ASM_SYSTEM_H_ */
