/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    28.05.2014
 */

#include <unistd.h>
#include <stdlib.h>
#include <embox/cmd.h>

EMBOX_CMD(test_fork_main);

static pid_t local_data;

static int test_fork_main(int argc, char *argv[]) {
	pid_t pid;
	int cnt = 5;
	int res = 0;

	pid = fork();
	if (pid == -1) {
		exit(2);
	}

	local_data = getpid();

	while (cnt--) {
		if (local_data != getpid()) {
			res = 3;
			break;
		}
		sleep(0);
	}

	if (pid) {
		int res;

		wait(&res);
		if (!res) {
			res = 4;
		}
	}

	exit(res);
}
