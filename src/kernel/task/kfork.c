/**
 * @file
 *
 * @date Jan 9, 2014
 * @author: Anton Bondarev
 */
#include <sys/types.h>

#include <kernel/task.h>

static void *kfork_trampoline(void *arg) {
	return 0;
}

pid_t kfork(void) {
	struct task *parent_task;
	struct thread *par_thread;
	pid_t child_pid;


	parent_task = task_self();
	par_thread = thread_self();

	child_pid = new_task(parent_task->task_name, kfork_trampoline, par_thread);

	return child_pid;

}
