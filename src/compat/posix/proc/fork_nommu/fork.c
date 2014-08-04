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
	memcpy(&ptregs, adrspc->pt_entry, sizeof(ptregs));
	sysfree(adrspc->pt_entry);
	adrspc->pt_entry = NULL;

	ptregs_retcode_jmp(&ptregs, 0);
	panic("%s returning", __func__);
}

void __attribute__((noreturn)) fork_body(struct pt_regs *ptregs) {
	struct addr_space *adrspc, *child_adrspc;
	pid_t child_pid;

	adrspc = fork_addr_space_get(task_self());
	if (!adrspc) {
		adrspc = fork_addr_space_create(thread_self(), NULL);
		fork_set_addr_space(task_self(), adrspc);
	}

	child_adrspc = fork_addr_space_create(thread_self(), adrspc);
	fork_addr_space_store(child_adrspc);

	child_adrspc->pt_entry = sysmalloc(sizeof(*adrspc->pt_entry));
	assert(child_adrspc->pt_entry);
	memcpy(child_adrspc->pt_entry, ptregs, sizeof(*child_adrspc->pt_entry));

	sched_lock();
	{
		child_pid = new_task("", fork_child_trampoline, NULL);
		fork_set_addr_space(task_table_get(child_pid), child_adrspc);
	}
	sched_unlock();

	ptregs_retcode_jmp(ptregs, child_pid);

	panic("%s returning", __func__);
}

