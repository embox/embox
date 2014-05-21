/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 */

#include <unistd.h>
#include <setjmp.h>

#include <kernel/task.h>
#include <kernel/task/resource/task_vfork.h>
#include <hal/vfork.h>


extern int task_prepare(const char *name);
extern int task_start(struct task *task, void * (*run)(void *), void *arg);

int task_is_vforking(struct task *task) {
	return task->status & TASK_STATUS_IN_VFORK;
}

void vfork_begin(struct task *task) {
	task->status |= TASK_STATUS_IN_VFORK;
}

void vfork_child_done(struct task *task, void * (*run)(void *), void *arg) {
	struct task_vfork *task_vfork;
	struct task *child;
	int tid;

	task_vfork = task_resource_vfork(task);
	tid = task_vfork->vforked_pid;

	child = task_table_get(tid);
	task_start(child, run, arg);

	ptregs_retcode(&task_vfork->ptregs, tid);
	vfork_leave(&task_vfork->ptregs);
}

void vfork_finish(struct task *task) {
	task = task_self();
	((struct task *)task)->status &= ~TASK_STATUS_IN_VFORK;
}

pid_t vfork_body(struct pt_regs *ptregs) {
	struct task *task;
	struct task_vfork *task_vfork;

	task = task_self();

	vfork_begin(task);

	task_vfork = task_resource_vfork(task);
	memcpy(&task_vfork->ptregs, ptregs, sizeof(task_vfork->ptregs));

	task_vfork->vforked_pid = task_prepare(NULL);
	ptregs_retcode(ptregs, 0);
	vfork_leave(ptregs);
	return 0;
}
