/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 * @author Anton Kozlov
 * 	- shell invocation
 */

#include <string.h>
#include <unistd.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <hal/context.h>

#include <kernel/task/resource/module_ptr.h>
#include <kernel/task/resource/task_argv.h>

extern void vfork_release_parent(void);
extern int exec_call(void);

#define EXEC_LEN 32

static void exec_trampoline(void) {
	sched_unlock();

	vfork_release_parent();

	_exit(exec_call());
}

int execv(const char *path, char *const argv[]) {
	struct context oldctx;
	struct thread *t;
	struct task *task;

	strncpy(exec_path, path, sizeof(exec_path) - 1);

	task = task_self();
	task_resource_exec(task, path, argv);

	sched_lock();
	t = thread_self();

	context_init(&t->context, true);
	context_set_entry(&t->context, exec_trampoline);
	context_set_stack(&t->context,
			thread_stack_get(t) + thread_stack_get_size(t));

	context_switch(&oldctx, &t->context);

	return 0;
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	return execv(path, argv);
}
