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
#include <compiler.h>

static void *vfork_body_exit_stub(void *arg) {
	_exit(*((int*) arg));
}

/*
 * If a multi-threaded task calls fork(), the new task will contain a replica
 * only of the calling thread and its entire address space.
 * Warning: If a thread that is not calling fork() holds a resource, that
 * resource is never released in the child task.
 */
void _NORETURN vfork_body(struct pt_regs *ptregs) {
	struct task *child;
	pid_t child_pid;
	struct task_vfork *task_vfork;
	int res;

	/* create task description but not start its main thread */
	child_pid = task_prepare("");
	if (0 > child_pid) {
		/* error */
		ptregs_retcode_err_jmp(ptregs, -1, -child_pid);
		panic("vfork_body returning");
	}
	child = task_table_get(child_pid);
	/* save ptregs for parent return from vfork() */
	task_vfork = task_resource_vfork(child->parent);

	memcpy(&task_vfork->ptregs, ptregs, sizeof(task_vfork->ptregs));

	res = vfork_child_start(child);

	if (res < 0) {
		/* Could not start child process */

		/* Exit child task */
		vfork_child_done(child, vfork_body_exit_stub, &res);

		/* Return to the parent */
		ptregs_retcode_err_jmp(&task_vfork->ptregs, -1, -res);
	}

	panic("vfork_body returning");
}
