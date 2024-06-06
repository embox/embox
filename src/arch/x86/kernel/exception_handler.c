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

#include <asm/hal/env/traps_core.h>
#include <asm/linkage.h>
#include <asm/ptrace.h>
#include <asm/traps.h>
#include <kernel/irq.h>
#include <kernel/panic.h>

__trap_handler __exception_table[0x20];

fastcall void exception_handler(pt_regs_t *st) {
	if (NULL != __exception_table[st->trapno]) {
		__exception_table[st->trapno](st->trapno, st);
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
	    st->trapno, st->err, st->eax, st->ebx, st->ecx, st->edx, st->gs, st->fs,
	    st->es, st->ds, st->edi, st->esi, st->ebp, st->eip, st->cs, st->eflags,
	    st->esp, st->ss);
}
