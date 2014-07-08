/**
 * @file
 *
 * @date May 23, 2014
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <setjmp.h>

#include <kernel/panic.h>
#include <kernel/task.h>
#include <kernel/task/resource/task_vfork.h>

#include <kernel/sched/sched_lock.h>

#include <hal/vfork.h>
#include <hal/ptrace.h>

extern int task_prepare(const char *name);
extern int task_start(struct task *task, void * (*run)(void *), void *arg);

void thread_set_task(struct thread *thread, struct task *task) {
	thread->task = task;
}

void vfork_begin(struct task *child, struct pt_regs *ptregs) {
	struct task_vfork *task_vfork;

	/* save ptregs for parent return from vfork() */
	task_vfork = task_resource_vfork(child);
	memcpy(&task_vfork->ptregs, ptregs, sizeof(task_vfork->ptregs));
//	task_vfork->vforked_task = child;
	thread_set_task(thread_self(), child);

	/* mark as vforking */
	task_vfork_start(child);
}

void vfork_child_done(struct task *child, void * (*run)(void *)) {
	struct task_vfork *vfork_data;

	vfork_data = task_resource_vfork(child);

	// FIXME : pass args?
	task_start(child, run, NULL);

	ptregs_retcode(&vfork_data->ptregs, child->tsk_id);
	thread_set_task(thread_self(), child->parent);
	task_vfork_end(child);

	ptregs_jmp(&vfork_data->ptregs);
}

void __attribute__((noreturn)) vfork_body(struct pt_regs *ptregs) {
	struct task *child;

	sched_lock();
	{
		pid_t child_pid = task_prepare("");
		if (0 > child_pid) {
			/* error */
			ptregs_retcode(ptregs, -1);
			sched_unlock();
			ptregs_jmp(ptregs);
		}
		child = task_table_get(child_pid);

		vfork_begin(child, ptregs);
	}
	sched_unlock();

	ptregs_retcode(ptregs, 0);
	ptregs_jmp(ptregs);
	panic("vfork_body returning");
}
