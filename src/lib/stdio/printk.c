/**
 * @file
 * @brief
 *
 * @date 10.11.10
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <stdarg.h>
#include <types.h>
#include <kernel/diag.h>

int __print(void (*printchar_handler)(char **str, int c),
		char **out, const char *format, va_list args);

static void diag_printchar(char **str, int c) {
	if (str) {
		**str = c;
		++(*str);
	} else {
		diag_putc(c);
	}
}

int printk(const char *format, ...) {
	int ret;
	va_list args;

	va_start(args, format);
	ret = __print(diag_printchar, 0, format, args);
	va_end(args);

	return ret;
}
