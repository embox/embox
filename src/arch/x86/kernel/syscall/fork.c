/**
 * @file
 *
 * @date 21.11.2012
 * @author Anton Bulychev
 */

#include <asm/linkage.h>
#include <asm/traps.h>

#include <kernel/usermode.h>

asmlinkage int sys_fork(struct pt_regs regs) {
	// TODO: sched_lock, ipl_enable ...
	return user_task_create((void *) regs.eip, (void *) regs.esp);
}
