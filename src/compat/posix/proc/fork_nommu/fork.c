/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.05.2014
 */

#include <kernel/panic.h>
#include <kernel/sched/sched_lock.h>
#include <hal/ptrace.h>
#include <compiler.h>

#include <mem/page.h>
#include <mem/phymem.h>
#include <mem/sysmalloc.h>
#include "fork_copy_addr_space.h"

#include <kernel/task/resource/task_fork.h>

#include <kernel/thread/thread_stack.h>

static void *fork_child_trampoline(void *arg) {
	struct addr_space *adrspc;
	struct thread *par_t, *cur_t;

	adrspc = fork_addr_space_get(task_self());
	par_t = adrspc->parent_thread;
	cur_t = thread_self();

	/* Set stack to parent thread stack */
	thread_stack_set(cur_t, thread_stack_get(par_t));
	thread_stack_set_size(cur_t, thread_stack_get_size(par_t));
	fork_stack_store(adrspc);

	ptregs_retcode_jmp(&adrspc->pt_entry, 0);
	panic("%s returning", __func__);
}

void _NORETURN fork_body(struct pt_regs *ptregs) {
	struct addr_space *adrspc;
	struct addr_space *child_adrspc;
	struct task *parent;
	pid_t child_pid;
	struct task *child;

	assert(ptregs);

	parent = task_self();
	assert(parent);

	child_pid = task_prepare("");
	if (0 > child_pid) {
		/* error */
		ptregs_retcode_err_jmp(ptregs, -1, child_pid);
		panic("%s returning", __func__);
	}

	adrspc = fork_addr_space_get(parent);
	if (!adrspc) {
		adrspc = fork_addr_space_create(NULL);
		fork_addr_space_set(parent, adrspc);
	}

	/* Save the stack of the current thread */
	fork_stack_store(adrspc);

	child_adrspc = fork_addr_space_create(adrspc);
	fork_addr_space_store(child_adrspc);
	memcpy(&child_adrspc->pt_entry, ptregs, sizeof(*ptregs));

	sched_lock();
	{
		child = task_table_get(child_pid);
		task_start(child, fork_child_trampoline, NULL);
		fork_addr_space_set(child, child_adrspc);
	}
	sched_unlock();

	ptregs_retcode_jmp(ptregs, child_pid);
	panic("%s returning", __func__);
}

