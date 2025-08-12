#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <framework/cmd/api.h>
#include <kernel/cpu/cpu.h>
#include <kernel/thread.h>

struct cmd_arg {
	const struct cmd *cmd;
	char **argv;
	int argc;
};

static void *run_cmd(void *arg) {
	struct cmd_arg *cmd_arg;

	cmd_arg = arg;

	cmd_exec(cmd_arg->cmd, cmd_arg->argc, cmd_arg->argv);

	return 0;
}

int main(int argc, char *argv[]) {
	struct thread *t;
	struct cmd_arg cmd_arg;

	if (argc < 2) {
		return -1;
	}

	cmd_arg.cmd = cmd_lookup(argv[1]);
	if (!cmd_arg.cmd) {
		return -1;
	}

	cmd_arg.argc = argc - 1;
	cmd_arg.argv = &argv[1];

	t = thread_create(THREAD_FLAG_SUSPENDED, run_cmd, &cmd_arg);
	if (!t) {
		return 0;
	}

	cpu_bind(1, t);

	thread_launch(t);

	thread_join(t, NULL);

	return 0;
}
