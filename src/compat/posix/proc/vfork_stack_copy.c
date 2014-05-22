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


#include <kernel/sched/sched_lock.h>

#include <hal/vfork.h>




extern int task_prepare(const char *name);
extern int task_start(struct task *task, void * (*run)(void *), void *arg);

int task_is_vforking(struct task *task) {
	return task->status & TASK_STATUS_IN_VFORK;
}

void vfork_begin(struct task *task) {
	struct task *child;

	task->status |= TASK_STATUS_IN_VFORK;
	child = task_self();
	child->status |= TASK_STATUS_IN_VFORK;
}

void vfork_child_done(struct task *child, void * (*run)(void *), void *arg) {
	struct task_vfork *parent_vfork;
	struct task *parent;

	parent = child->parent;

	parent_vfork = task_resource_vfork(parent);

	task_start(child, run, arg);

	ptregs_retcode(&parent_vfork->ptregs, child->tsk_id);
	child->status &= ~TASK_STATUS_IN_VFORK;
	parent->status &= ~TASK_STATUS_IN_VFORK;

	vfork_leave(&parent_vfork->ptregs);
}

void vfork_finish(struct task *task) {
	task = task_self();
	((struct task *)task)->status &= ~TASK_STATUS_IN_VFORK;
}

pid_t vfork_body(struct pt_regs *ptregs) {
	struct task *task, *child;
	struct task_vfork *task_vfork;//, *child_vfork;

	task = task_self();

	task_vfork = task_resource_vfork(task);

	memcpy(&task_vfork->ptregs, ptregs, sizeof(task_vfork->ptregs));

	sched_lock();
	{

		task_vfork->vforked_pid = task_prepare(NULL);
		child = task_table_get(task_vfork->vforked_pid);
		task_vfork->vforked_task = child;
		//child_vfork = task_resource_vfork(child);
		//memcpy(&child_vfork->ptregs, ptregs, sizeof(child_vfork->ptregs));

		vfork_begin(task);
	}


	ptregs_retcode(ptregs, 0);
	vfork_leave(ptregs);
	return 0;
}
