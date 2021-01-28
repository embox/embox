
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "cnc_ipc.h"

int main(int argc, char *argv[]) {
	int pipe_fds[2];
	char ipc_var[CNCIPC_ENV_LEN];
	char *cmd[] = { "httpd", "http_admin", NULL };
	int err;

	if ((err = pipe(pipe_fds))) {
		return err;
	}

	snprintf(ipc_var, sizeof(ipc_var), CNCIPC_IN  "=%d", pipe_fds[0]);
	putenv(ipc_var);
	snprintf(ipc_var, sizeof(ipc_var), CNCIPC_OUT "=%d", pipe_fds[1]);
	putenv(ipc_var);

	return execv("httpd", cmd);
}
