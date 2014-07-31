/**
 * @file
 *
 * @date Jun 6, 2014
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>

int fioctl(FILE *file, int request, ...) {
	va_list args;
	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}
	va_start(args, request);
	return ioctl(file->fd, request, args);
}
