/**
 * @file
 *
 * @date 07.11.2012
 * @author Anton Bulychev
 */

#include <asm/ptrace.h>

#include <kernel/syscall_table.h>

void syscall_handler(struct pt_regs *regs) {
	uint32_t result;
	uint32_t (*sys_func)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) =
			 SYSCALL_TABLE[regs->ins[0]];

	result = sys_func(regs->ins[1], regs->ins[2], regs->ins[3],
			    regs->ins[4], regs->ins[5]);

	regs->ins[0] = result;
	regs->pc = regs->npc;
	regs->npc = regs->npc + 4;
}
