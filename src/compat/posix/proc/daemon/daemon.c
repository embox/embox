/**
 * @file
 * @brief
 *
 * @date 07.03.23
 * @author Aleksey Zhmulin
 */
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int daemon(int nochdir, int noclose) {
	pid_t pid;
	int fd;

	pid = vfork();
	if (pid < 0) {
		return -1;
	}
	if (pid > 0) {
		/* Use _exit() to avoid doing atexit() stuff. */
		_exit(0);
	}

	if (!noclose && (fd = open("/dev/null", O_RDWR, 0)) != -1) {
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > 2) {
			close(fd);
		}
	}
	return 0;
}
