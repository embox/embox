/**
 * @file
 *
 * @date Sep 3, 2021
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>

#include "printf_impl.h"

struct printchar_handler_data {
	int fd;
};

static int file_printchar(struct printchar_handler_data *d, int c) {
	uint8_t buf;

	buf = (uint8_t)c & 0xFF;

	return write(d->fd, &buf, 1);
}

int dprintf(int fildes, const char *format, ...) {
	int ret;
	struct printchar_handler_data data;
	va_list args;

	assert(format != NULL);
	data.fd = fildes;

	va_start(args, format);
	ret = __print(file_printchar, &data, format, args);
	va_end(args);

	return ret;
}
