/**
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */

#include <embox/unit.h>

#include <asm/ptrace.h>
#include <asm/entry.h>

#include <kernel/syscall_table.h>

EMBOX_UNIT_INIT(mips_syscall_init);

void mips_c_syscall_handler(pt_regs_t *regs) {
	uint32_t result;
	uint32_t (*sys_func)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) =
			 SYSCALL_TABLE[regs->reg[0]];

	result = sys_func(regs->reg[0], regs->reg[1], regs->reg[2],
			    regs->reg[3], regs->reg[4]);

	regs->reg[0] = result;
	regs->pc += 4;
}

static int mips_syscall_init(void) {
	/* set interrupt handler exception */
	mips_exception_setup(MIPS_EXCEPTION_TYPE_SYSCALL, mips_c_syscall_handler);

	return 0;
}
