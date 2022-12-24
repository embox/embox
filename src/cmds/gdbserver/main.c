/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.12.22
 */

#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <framework/cmd/api.h>
#include <framework/cmd/types.h>
#include <hal/test/traps_core.h>

#include "remote.h"
#include "breakpoint.h"

extern int gdbstub_err;
extern int gdbstub(uint32_t nr, void *regs);

int main(int argc, char *argv[]) {
	const struct cmd *cmd;
	char *delim;
	int prog_ret;
	int ret;

	if (argc < 3) {
		ret = -EINVAL;
		goto out;
	}

	delim = strchr(argv[1], ':');
	if ((delim == NULL) || (delim[1] == '\n')) {
		ret = -EINVAL;
		goto out;
	}
	*delim = '\0';

	ret = remote_open(argv[1], delim + 1);
	if (ret < 0) {
		goto out;
	}

	cmd = cmd_lookup(argv[2]);
	if (cmd == NULL) {
		ret = -ENOENT;
		goto out;
	}

	if (insert_breakpoint(cmd->exec)) {
		set_fault_handler(BREAKPOINT, gdbstub);
		prog_ret = cmd_exec(cmd, argc - 2, &argv[2]);
		ret = gdbstub_err;
		set_fault_handler(BREAKPOINT, NULL);
	}

out:
	remote_close();

	if (ret == 0) {
		remove_all_breakpoints();
		printf("\nProgram exited with status %i\n", prog_ret);
	}
	else if (ret == -EINVAL) {
		printf("Usage: %s [HOST]:[PORT] [PROG] [ARGS ...]\n", argv[0]);
	}
	return ret;
}
