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
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <hal/context.h>

#include <kernel/task/resource/module_ptr.h>

extern void vfork_release_parent(void);
extern int exec_call(char *path, char *argv[], char *envp[]);

#define EXEC_LEN 32
static char exec_path[EXEC_LEN];

#if 0
#define EXEC_ARGC 8
static int  exec_argc;
static char exec_argv[EXEC_ARGC][EXEC_LEN];
#endif

static void exec_trampoline(void) {
	char *path = exec_path;

	sched_unlock();

	vfork_release_parent();

	_exit(exec_call(path, NULL, NULL));
}

int execv(const char *path, char *const argv[]) {
	struct context oldctx;
	struct thread *t;

	strncpy(exec_path, path, sizeof(exec_path) - 1);

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
