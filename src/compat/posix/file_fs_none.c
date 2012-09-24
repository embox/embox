/**
 * @file
 *
 * @brief
 *
 * @date 06.11.2011
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <prom/diag.h>
#include <util/array.h>

int open(const char *path, int __oflag, ...) {
	return -1;
}

int close(int fd) {
	return -1;
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
	const char *ch = buf;
	int cnt = nbyte;

	if (fd != 1 && fd != 2) {
		return -1;
	}

	while (cnt --) {
		diag_putc(*ch);
		ch++;
	}
	return nbyte;

}

ssize_t read(int fd, void *buf, size_t nbyte) {
	char *ch = buf;
	int cnt = nbyte;

	if (fd != 0) {
		return -1;
	}

	while (cnt --) {
		*ch = diag_getc();
		ch++;
	}
	return nbyte;

}

int lseek(int fd, long int offset, int origin) {
	return -1;
}

int ioctl(int fd, int request, ...) {
	return -1;
}

int fsync(int fd) {
	return 0;
}
