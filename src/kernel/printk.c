/**
 * @file
 * @brief Print a formatted message to the kernel console.
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdarg.h>
#include <stdio.h>

#include <kernel/printk.h>

/**
 * Print a formatted message to the kernel console.
 * @param format C string in Kernel
 * @param additional_arguments
 *   Depending on the format string, the function may expect a sequence of
 *   additional arguments, each containing one value to be inserted instead
 *   of each %-tag specified in the format parameter, if any. There should be
 *   the same number of these arguments as the number of %-tags that expect a
 *   value.
 */
#if 1
int __attribute__ ((format (printf, 1, 2))) printk(const char *format, ...) {
	va_list args;
	int ret;

	va_start(args, format);
	ret = printf(format, args);
	va_end(args);

	return ret;
}
#endif
