/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.01.23
 */
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>

ssize_t gdbserver_read(int fd, char *dst, size_t nbyte) {
	ssize_t nread;

	do {
		nread = read(fd, dst, 1);
	} while ((nread == 1) && (*dst != '$'));

	if (nread == 1) {
		while (nread < nbyte) {
			if (1 != read(fd, ++dst, 1)) {
				break;
			}
			nread++;
			if ((*dst == '#') && (nread + 2 < nbyte)) {
				nbyte = nread + 2;
			}
		}
	}
	return nread;
}

ssize_t gdbserver_write(int fd, const char *src, size_t nbyte) {
	return write(fd, src, nbyte);
}

int gdbserver_prepare_connection(const char *tty) {
	struct termios t;
	int ret;
	int fd;

	ret = fd = open(tty, O_RDWR);
	if (fd == -1) {
		ret = -errno;
		goto out;
	}

	if (-1 == tcgetattr(fd, &t)) {
		ret = -errno;
		goto out;
	}

	t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	t.c_oflag &= ~(OPOST);
	t.c_cflag |= (CS8);
	t.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;

	if (-1 == tcsetattr(fd, TCSANOW, &t)) {
		ret = -errno;
		goto out;
	}

	printf("Remote debugging using %s\n", tty);

out:
	return ret;
}
