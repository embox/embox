/**
 * @file
 *
 * @brief
 *
 * @date 21.09.2011
 * @author Anton Bondarev
 */

#include <inttypes.h>
#include <stddef.h>

#include <asm/cr_regs.h>
#include <asm/flags.h>
#include <asm/hal/env/traps_core.h>
#include <asm/linkage.h>
#include <asm/ptrace.h>
#include <asm/traps.h>
#include <kernel/panic.h>

__trap_handler __exception_table[0x20];

fastcall void x86_excpt_handler(pt_regs_t *regs) {
#ifdef __SSE__
	uint32_t cr4;
	if (regs->trapno == X86_T_INVALID_OPCODE) {
		cr4 = get_cr4();
		if (!(cr4 & X86_CR4_OSFXSR)) {
			set_cr4(cr4 | X86_CR4_OSFXSR | X86_CR4_OSXMMEXCPT);
			return;
		}
	}
#endif

	if (NULL != __exception_table[regs->trapno]) {
		__exception_table[regs->trapno](regs->trapno, regs);
		return;
	}

	panic("EXCEPTION [0x%" PRIx32 "]: error = %08" PRIx32 "\n"
	      "EAX    =%" PRIx32 "\n"
	      "EBX    =%" PRIx32 "\n"
	      "ECX    =%" PRIx32 "\n"
	      "EDX    =%" PRIx32 "\n"
	      "GS     =%" PRIx32 "\n"
	      "FS     =%" PRIx32 "\n"
	      "ES     =%" PRIx32 "\n"
	      "DS     =%" PRIx32 "\n"
	      "EDI    =%" PRIx32 "\n"
	      "ESI    =%" PRIx32 "\n"
	      "EBP    =%" PRIx32 "\n"
	      "EIP    =%" PRIx32 "\n"
	      "CS     =%" PRIx32 "\n"
	      "EFLAGS =%" PRIx32 "\n"
	      "ESP    =%" PRIx32 "\n"
	      "SS     =%" PRIx32 "\n",
	    regs->trapno, regs->err, regs->eax, regs->ebx, regs->ecx, regs->edx,
	    regs->gs, regs->fs, regs->es, regs->ds, regs->edi, regs->esi, regs->ebp,
	    regs->eip, regs->cs, regs->eflags, regs->esp, regs->ss);
}
