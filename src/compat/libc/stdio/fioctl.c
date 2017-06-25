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
#include "file_struct.h"

int fioctl(FILE *file, int request, ...) {
	va_list args;
	int res;
	void *data;

	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	va_start(args, request);
	data = va_arg(args, void*);
	res = ioctl(file->fd, request, data);
	va_end(args);

	return res;
}
