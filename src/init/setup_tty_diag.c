/**
 * @file
 * @brief
 *
 * @date 06.03.14
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */

#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#define DIAG_NAME "diag"

extern int diag_fd(void);

const char *setup_tty(const char *dev_name) {
	int fd;

	fd = diag_fd();
	if (fd < 0) {
		return NULL;
	}

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	if (fd > STDERR_FILENO) {
		close(fd);
	}

	return DIAG_NAME;
}
