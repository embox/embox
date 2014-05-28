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
#include <framework/cmd/api.h>
#include <cmd/shell.h>

extern void vfork_release_parent(void);

#define EXEC_LEN 32
static char exec_path[EXEC_LEN];

#if 0
#define EXEC_ARGC 8
static int  exec_argc;
static char exec_argv[EXEC_ARGC][EXEC_LEN];
#endif

static void exec_trampoline(void) {
	const struct shell *sh;
	const char *default_shells[] = { "/bin/sh", "sh", NULL };
	const char **shellp;
	char *path = exec_path;
	int ecode;

	sched_unlock();

	vfork_release_parent();

	sh = shell_lookup(path);
	if (!sh) {
		for (shellp = default_shells; *shellp != NULL; shellp++) {
			if (!strcmp(*shellp, path)) {
				sh = shell_lookup("tish");
				break;
			}
		}
	}

	/* FIXME pass argv to shell_exec */
	if (sh) {
		ecode = shell_run(sh);
	} else {
		const struct cmd *cmd;

		cmd = cmd_lookup(path);

		if (cmd) {
			char *argv[] = { path, NULL };

			task_self_module_ptr_set(cmd2mod(cmd));
			ecode = cmd_exec(cmd, 1, argv);
		} else {
			ecode = ENOENT;
		}

	}

	_exit(ecode);
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
