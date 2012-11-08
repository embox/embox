/**
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */

#include <asm/ptrace.h>
#include <asm/entry.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(mips_syscall_init);

void mips_c_syscall_handler(pt_regs_t *regs) {
	regs->pc += 4;
}

static int mips_syscall_init(void) {
	/* set interrupt handler exception */
	mips_exception_setup(MIPS_EXCEPTION_TYPE_IRQ, mips_c_syscall_handler);

	return 0;
}
