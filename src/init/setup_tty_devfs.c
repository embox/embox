/**
 * @file
 * @brief Runs start script
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>

int setup_tty(const char *dev_name) {
	int fd;
	char full_name[PATH_MAX];

	putenv("TERM=emterm");

	if (strlen(dev_name) == 0) {
		return -EINVAL;
	}

	strncpy(full_name, "/dev/", sizeof(full_name));
	strcat(full_name, dev_name);

	if (-1 == (fd = open(full_name, O_RDWR))) {
		return -errno;
	}

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	if (fd > 2) {
		close(fd);
	}

	return 0;
}
