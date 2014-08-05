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

#include <mem/page.h>
#include <mem/phymem.h>
#include <mem/sysmalloc.h>
#include "fork_copy_addr_space.h"

#include <kernel/task/resource/task_fork.h>

static void *fork_child_trampoline(void *arg) {
	struct addr_space *adrspc;
	struct pt_regs ptregs;

	adrspc = fork_addr_space_get(task_self());
	memcpy(&ptregs, &adrspc->pt_entry, sizeof(ptregs));
	//sysfree(adrspc->pt_entry);
	//adrspc->pt_entry = NULL;

	ptregs_retcode_jmp(&ptregs, 0);
	panic("%s returning", __func__);
}

void __attribute__((noreturn)) fork_body(struct pt_regs *ptregs) {
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
		ptregs_retcode_err_jmp(ptregs, -1, -child_pid);
		panic("fork_body returning");
	}

	adrspc = fork_addr_space_get(parent);
	if (!adrspc) {
		adrspc = fork_addr_space_create(thread_self(), NULL);
		fork_addr_space_set(parent, adrspc);
	}

	child_adrspc = fork_addr_space_create(thread_self(), adrspc);
	fork_addr_space_store(child_adrspc);

	//child_adrspc->pt_entry = sysmalloc(sizeof(*adrspc->pt_entry));
	//assert(child_adrspc->pt_entry);
	memcpy(&child_adrspc->pt_entry, ptregs, sizeof(child_adrspc->pt_entry));

	sched_lock();
	{
		//child_pid = new_task("", fork_child_trampoline, NULL);
		child = task_table_get(child_pid);
		task_start(child, fork_child_trampoline, NULL);

		fork_addr_space_set(child, child_adrspc);
	}
	sched_unlock();

	if (child_pid > 0) {
		ptregs_retcode_jmp(ptregs, child_pid);
	} else {
		ptregs_retcode_err_jmp(ptregs, -1, -child_pid);
	}

	panic("%s returning", __func__);
}

