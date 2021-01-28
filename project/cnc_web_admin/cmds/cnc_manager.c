/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    17.02.2015
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "cnc_ipc.h"

static int cnc_ctrl_ipc_init(void) {
	const char *cnc_out = getenv(CNCIPC_OUT);
	return cnc_out ? atoi(cnc_out) : -1;
}

int main(int argc, char *argv[]) {

	if (0 == strncmp(getenv("QUERY_STRING"), "run", strlen("run"))) {
		char *arg[] = { "cnc_control", "test", NULL };
		int ipc_fd = cnc_ctrl_ipc_init();

		printf("HTTP/1.1 %d %s\r\n"
			"Content-Type: %s\r\n"
			"Connection: close\r\n"
			"\r\n", 200, "OK", "text/plain");

		dup2(ipc_fd, STDOUT_FILENO);
		close(STDIN_FILENO);
		dup2(ipc_fd, STDERR_FILENO);

		return execv(arg[0], arg);
	}
	return 0;
}
