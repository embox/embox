/**
 * @file
 * @brief
 *
 * @author
 * @date
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

int vprintf(const char *format, va_list args) {
	return vfprintf(stdout, format, args);
}

int printf(const char *format, ...) {
	int ret;
	va_list args;

	assert(format != NULL);

	va_start(args, format);
	ret = vprintf(format, args);
	va_end(args);

	return ret;
}
