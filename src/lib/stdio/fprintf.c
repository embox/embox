/**
 * @file
 * @brief
 *
 * @date 01.12.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

struct printchar_handler_data;
extern int __print(void (*printchar_handler)(struct printchar_handler_data *d, int c),
		struct printchar_handler_data *printchar_data,
		const char *format, va_list args);

struct printchar_handler_data {
	FILE *file;
};

static void file_printchar(struct printchar_handler_data *d, int c) {
	assert(d != NULL);
	assert(d->file != NULL);

	fputc(c, d->file);
}

int vfprintf(FILE *file, const char *format, va_list args) {
	struct printchar_handler_data data;

	assert(file != NULL);
	assert(format != NULL);

	data.file = file;

	return __print(file_printchar, &data, format, args);
}

int fprintf(FILE *file, const char *format, ...) {
	int ret;
	va_list args;

	assert(file != NULL);
	assert(format != NULL);

	va_start(args, format);
	ret = vfprintf(file, format, args);
	va_end(args);

	return ret;
}
