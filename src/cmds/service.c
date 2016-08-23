/**
 * @file
 * @brief Starts services
 *
 * @date 16.04.16
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static void srv_print_services(void) {
	printf("PRINT LIST OF SERVICES IS NOT SUPPORTED YET\n");
}

static int service_run(const char *path, char *const argv[]) {
	pid_t pid;

	pid = vfork();
	if (pid < 0) {
		int err = errno;
		printf("vfork() error(%d): %s", err, strerror(err));
		return -err;
	}

	if (pid == 0) {
		execv(path, argv);
		exit(1);
	}

	return 0;
}

int main(int argc, char **argv) {
	const char *command;

	if (argv[1] == NULL) {
		srv_print_services();
		return 0;
	}
	command = argv[1];
	printf("Starting service: %s\n", command);

	return service_run(command, &argv[1]);
}
