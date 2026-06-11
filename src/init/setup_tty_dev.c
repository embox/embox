/**
 * @file
 * @brief
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <drivers/char_dev.h>
#include <util/log.h>

const char *setup_tty(const char *dev_name) {
	int fd;

	fd = char_dev_open(dev_name, O_RDWR);
	if (fd < 0) {
		if (fd == -ENOENT) {
			log_error("tty device not found");
		}
		else {
			log_error(strerror(-fd));
		}
		return NULL;
	}

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	if (fd > 2) {
		close(fd);
	}

	return dev_name;
}
