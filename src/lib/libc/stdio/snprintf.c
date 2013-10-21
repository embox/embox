/**
 * @file
 *
 * @date 14.12.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "printf_impl.h"

struct printchar_handler_data {
	char *str;
	size_t left;
};

static void strn_printchar(struct printchar_handler_data *d, int c) {
	assert(d != NULL);
	assert((d->str != NULL) || (d->left == 0));

	if (d->left) {
		*d->str++ = c;
		--d->left;
	}
}

int vsnprintf(char *str, size_t size, const char *format, va_list args) {
	int ret;
	struct printchar_handler_data data;

	assert((str != NULL) || (size == 0));
	assert(format != NULL);

	data.str = str;
	data.left = size ? size - 1 : 0;
	ret = __print(strn_printchar, &data, format, args);
	if (size) *data.str = '\0';

	return ret;
}

int snprintf(char *str, size_t size, const char *format, ...) {
	int ret;
	va_list args;

	assert((str != NULL) || (size == 0));
	assert(format != NULL);

	va_start(args, format);
	ret = vsnprintf(str, size, format, args);
	va_end(args);

	return ret;
}
