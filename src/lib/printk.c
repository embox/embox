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

#include <module/embox/compat/libc/stdio/print.h>

static void printk_printchar(struct printchar_handler_data *d, int c) {
	diag_putc(c);
}

int printk(const char *format, ...) {
	int ret;
	va_list args;

	assert(format != NULL);

	va_start(args, format);
	ret = __print(printk_printchar, NULL, format, args);
	va_end(args);

	return ret;
}

int vprintk(const char *format, va_list args) {
	return __print(printk_printchar, NULL, format, args);
}
