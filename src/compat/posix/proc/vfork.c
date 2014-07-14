/**
 * @file
 *
 * @date Jul 12, 2014
 * @author Anton Bondarev
 */
#include <unistd.h>

#include <kernel/panic.h>
#include <kernel/task.h>
#include <kernel/task/resource/task_vfork.h>

#include <kernel/sched/sched_lock.h>

#include <hal/vfork.h>
#include <hal/ptrace.h>

extern int vfork_callback(struct task *child);

void __attribute__((noreturn)) vfork_body(struct pt_regs *ptregs) {
	struct task *child;
	pid_t child_pid;
	struct task_vfork *task_vfork;
	int res;

	/* can vfork only in single thread application */
	assert(thread_self() == task_self()->tsk_main);

	/* create task description but not start its main thread */
	child_pid = task_prepare("");
	if (0 > child_pid) {
		/* error */
		ptregs_retcode_jmp(ptregs, child_pid);
		panic("vfork_body returning");
	}
	child = task_table_get(child_pid);
	/* save ptregs for parent return from vfork() */
	task_vfork = task_resource_vfork(child->parent);
	memcpy(&task_vfork->ptregs, ptregs, sizeof(task_vfork->ptregs));

	res = vfork_callback(child);
	//TODO error we must delete child task
	ptregs_retcode_jmp(ptregs, res);
	panic("vfork_body returning");

}
