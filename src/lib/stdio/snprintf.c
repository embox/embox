/**
 * @file
 *
 * @date 14.12.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <types.h>

struct printchar_handler_data;
extern int __print(void (*printchar_handler)(struct printchar_handler_data *d, int c),
		struct printchar_handler_data *printchar_data,
		const char *format, va_list args);

struct printchar_handler_data {
	char *str;
	size_t left;
};

static void strn_printchar(struct printchar_handler_data *d, int c) {
	assert(d != NULL);
	assert(d->str != NULL);

	if (d->left) {
		*d->str++ = c;
		--d->left;
	}
}

int vsnprintf(char *str, size_t size, const char *format, va_list args) {
	int ret;
	struct printchar_handler_data data;

	assert(str != NULL);
	assert(format != NULL);

	data.str = str;
	data.left = size ? size - 1 : 0;
	ret = __print(strn_printchar, &data, format, args);
	assert(data.str != NULL);
	if (size) *data.str = '\0';

	return ret;
}

int snprintf(char *str, size_t size, const char *format, ...) {
	int ret;
	va_list args;

	assert(str != NULL);
	assert(format != NULL);

	va_start(args, format);
	ret = vsprintf(str, format, args);
	va_end(args);

	return ret;
}
