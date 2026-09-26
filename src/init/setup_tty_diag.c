/**
 * @file
 * @brief Setup TTY with fallback to diag device
 *
 * @date 06.03.14
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */

#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define DIAG_NAME "diag"
#define DEV_PREFIX "/dev/"

extern int diag_fd(void);

static int try_open_dev(const char *dev_name) {
	char path[64];
	int fd;

	/* Try /dev/<dev_name> first */
	snprintf(path, sizeof(path), DEV_PREFIX "%s", dev_name);
	fd = open(path, O_RDWR);
	if (fd >= 0) {
		return fd;
	}

	/* Try the name as-is (might be a full path) */
	fd = open(dev_name, O_RDWR);
	return fd;
}

static void redirect_stdio(int fd) {
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	if (fd > STDERR_FILENO) {
		close(fd);
	}
}

const char *setup_tty(const char *dev_name) {
	int fd;

	/* If a device name is specified and it's not "diag", try to open it */
	if (dev_name != NULL && dev_name[0] != '\0'
			&& strcmp(dev_name, DIAG_NAME) != 0) {
		fd = try_open_dev(dev_name);
		if (fd >= 0) {
			redirect_stdio(fd);
			return dev_name;
		}
	}

	/* Fallback to diag device */
	fd = diag_fd();
	if (fd < 0) {
		return NULL;
	}

	redirect_stdio(fd);

	return DIAG_NAME;
}
