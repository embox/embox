/**
 * @file
 * @brief
 *
 * @date 10.11.10
 * @author Anton Bondarev
 */

#include <assert.h>
#include <drivers/diag.h>
#include <stdarg.h>
#include <stdio.h>
#include <framework/mod/options.h>

#include <module/embox/compat/libc/Print.h>

struct printchar_handler_data;

static void iodev_printchar(struct printchar_handler_data *d, int c) {
	diag_putc(c);
}

int printk(const char *format, ...) {
	int ret;
	va_list args;

	assert(format != NULL);

	va_start(args, format);
	ret = __print(iodev_printchar, NULL, format, args);
	va_end(args);

	return ret;
}
