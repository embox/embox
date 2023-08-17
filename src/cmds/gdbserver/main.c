/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.01.23
 */
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <cmd/shell.h>
#include <framework/cmd/api.h>
#include <framework/cmd/types.h>

#include "gdbserver.h"

int main(int argc, char *argv[]) {
	const struct shell *sh;
	const struct cmd *cmd;
	void *entry;
	pid_t pid;
	int status;
	int err;
	int fd;

	err = 0;
	fd = -1;

	if (argc < 3) {
		err = -EINVAL;
		goto out;
	}

	sh = shell_lookup(argv[2]);
	if (sh) {
		entry = sh->run;
	}
	else {
		cmd = cmd_lookup(argv[2]);
		if (!cmd) {
			err = -ENOENT;
			goto out;
		}
		entry = cmd->exec;
	}

	fd = gdbserver_prepare_connection(argv[1]);
	if (fd < 0) {
		err = fd;
		goto out;
	}

	gdbserver_start(fd, entry);

	pid = vfork();
	if (pid < 0) {
		err = -errno;
	}
	else if (pid == 0) {
		execv(argv[2], &argv[2]);
		exit(1);
	}
	else {
		waitpid(pid, &status, 0);
		assert(WIFEXITED(status));
		status = WEXITSTATUS(status);

		printf("\n%s exited with status %i\n", argv[2], status);
	}

	gdbserver_stop();

out:
	if (err == -EINVAL) {
		printf("Usage: %s [HOST]:[PORT] [PROG] [PROG_ARGS ...]\n", argv[0]);
		printf("       %s [TTY_DEV] [PROG] [PROG_ARGS ...]\n", argv[0]);
	}

	close(fd);

	return err;
}
