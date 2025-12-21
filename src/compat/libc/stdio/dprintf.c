/**
 * @file
 *
 * @date Sep 3, 2021
 * @author Anton Bondarev
 */

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>

#include "file_struct.h"

int vdprintf(int fd, const char *format, va_list args) {
	FILE f = {
	    .fd = fd,
	    .flags = O_WRONLY,
	};

	return vfprintf(&f, format, args);
}

int dprintf(int fd, const char *format, ...) {
	int ret;
	va_list args;

	va_start(args, format);
	ret = vdprintf(fd, format, args);
	va_end(args);

	return ret;
}
