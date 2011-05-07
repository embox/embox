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

int __print(void (*printchar_handler)(char **str, int c),
		char **out, const char *format, va_list args);

static void printchar(char **str, int c) {
	if (str) {
		**str = c;
		++(*str);
	} else {
		putchar(c);
	}
}

int printk(const char *format, ...) {
	int ret;
	va_list args;

	va_start(args, format);
	ret = __print(printchar, 0, format, args);
	va_end(args);

	return ret;
}
