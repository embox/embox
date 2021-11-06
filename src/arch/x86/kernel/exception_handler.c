/**
 * @file
 *
 * @brief
 *
 * @date 21.09.2011
 * @author Anton Bondarev
 */

#include <asm/traps.h>
#include <asm/linkage.h>
#include <kernel/panic.h>
#include <kernel/irq.h>
#include <kernel/task.h>
#include <stddef.h>

#include <asm/hal/env/traps_core.h>

__trap_handler __exception_table[0x20];

fastcall void exception_handler(pt_regs_t *st) {
	if(NULL != __exception_table[st->trapno]) {
		// Don't handle this if it is a Linux syscall, but the task isn't marked as an LKL task
		if(!(st->trapno == 0xd && task_self()->lkl_task == 0)) {
			__exception_table[st->trapno](st->trapno, st);
			return;
		}
	}

	panic("EXCEPTION [0x%x]: error = %08x\n"
		"EAX=%08x    EBX=%08x ECX=%08x EDX=%08x\n"
		" GS=%08x     FS=%08x  ES=%08x  DS=%08x\n"
		"EDI=%08x    ESI=%08x EBP=%08x EIP=%08x\n"
		" CS=%08x EFLAGS=%08x ESP=%08x  SS=%08x\n",
		st->trapno, st->err,
		st->eax, st->ebx, st->ecx, st->edx,
		st->gs, st->fs, st->es, st->ds,
		st->edi, st->esi, st->ebp, st->eip,
		st->cs, st->eflags, st->esp, st->ss
	);
}
