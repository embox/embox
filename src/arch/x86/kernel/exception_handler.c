/**
 * @file
 *
 * @brief
 *
 * @date 21.09.2011
 * @author Anton Bondarev
 */
#include <asm/traps.h>
#include <kernel/panic.h>
#include <kernel/irq.h>

void int0301HandlerHack(pt_regs_t*); // todo: A HACK! See http://code.google.com/p/embox/issues/detail?id=358

void exception_handler(pt_regs_t *st) {
   if (3 == st -> trapno || 1 == st -> trapno) {
//          ipl_enable(); // todo: isn't this another hack
          int0301HandlerHack(st);
//          ipl_disable();
    } else {
	panic("EXCEPTION [0x%x]:\n"
		"EAX=%08x ECX=%08x ECX=%08x EDX=%08x\n"
		"GS=%08x FS=%08x ES=%08x DS=%08x\n"
		"EDI=%08x ESI=%08x EBP=%08x CR2=%08x\n"
		"EIP=%08x CS=%08x EFLAGS=%08x ESP=%08x SS=%08x\n",
		st->trapno, st->eax, st->ebx, st->ecx, st->edx,
		st->gs, st->fs, st->es, st->ds,
		st->edi, st->esi, st->ebp, st->cr2,
		st->eip, st->cs, st->eflags, st->esp, st->ss
	);
   }
}
