/**
 * @file
 * @brief
 *
 * @date 10.11.10
 * @author Anton Bondarev
 */

#include <stdarg.h>
#include <stdio.h>
#include <prom/prom_printf.h>
#include <drivers/diag.h>

#include <module/embox/compat/libc/Print.h>

static void printchar(struct printchar_handler_data *d, int c) {
	diag_putc(c);
}

int prom_printf(const char *format, ...) {
	int ret;
	va_list args;

	va_start(args, format);
	ret = prom_vprintf(format, args);
	va_end(args);

	return ret;
}

int prom_vprintf(const char *format, va_list args) {
	return __print(printchar, 0, format, args);
}
