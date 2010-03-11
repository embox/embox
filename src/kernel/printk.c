/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdarg.h>
#include <stdio.h>

#include <kernel/printk.h>

#if 0
int __attribute__ ((format (printf, 1, 2))) printk(const char *format, ...) {
	va_list args;
	int ret;

	va_start(args, format);
	ret = printf(format, args);
	va_end(args);

	return ret;
}
#endif
