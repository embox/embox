/**
 * @file
 *
 * @date 21.11.2012
 * @author Anton Bulychev
 */

#include <asm/linkage.h>
#include <asm/traps.h>

#include <stdint.h>
#include <errno.h>
#include <string.h>

#include <kernel/irq.h>
#include <kernel/sched.h>
#include <kernel/task.h>
#include <mem/misc/pool.h>


/* Simultaneous number of task creation */
#define PT_REGS_POOL_SIZE 5

POOL_DEF(pt_regs_pool, struct pt_regs, PT_REGS_POOL_SIZE);

static void *fork_trampoline(void *data) {
	extern void fork_leave(void *regs);
	struct pt_regs regs;

	memcpy(&regs, data, sizeof(struct pt_regs));

	sched_lock();
	{
		pool_free(&pt_regs_pool, data);
	}
	sched_unlock();

	/* return 0 */
	regs.eax = 0;

	fork_leave(&regs);

	return NULL;
}

asmlinkage int sys_fork(struct pt_regs regs) {
	struct pt_regs *data;
	int res;

	sched_lock();
	{
		ipl_enable();

		if (!(data = pool_alloc(&pt_regs_pool))) {
			sched_unlock();
			return -EAGAIN;
		}

		memcpy(data, &regs, sizeof(struct pt_regs));

		res = new_task(task_get_name(task_self()), fork_trampoline, data);
	}
	sched_unlock();

	return res;
}
