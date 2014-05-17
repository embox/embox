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

	longjmp(task_vfork->vfrok_jmpbuf, tid);
}

void vfork_finish(struct task *task) {
	task = task_self();
	((struct task *)task)->status &= ~TASK_STATUS_IN_VFORK;
}

pid_t vfork(void) {
	struct task *task;
	struct task_vfork *task_vfork;
	int res;

	task = task_self();

	vfork_begin(task);

	task_vfork = task_resource_vfork(task);

	switch(res = setjmp(task_vfork->vfrok_jmpbuf)) {
	case 0:
		task_vfork->vforked_pid = task_prepare(NULL);
		return 0;
	default:
		vfork_finish(task);
		return res;
	}
}
