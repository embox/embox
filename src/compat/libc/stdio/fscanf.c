/**
 * @file
 *
 * @date Nov 3, 2020
 * @author Anton Bondarev
 */
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

extern int stdio_scan(const char **in, const char *fmt, va_list args);

FILE *__fscanf_file;

int fscanf(FILE *stream, const char *format, ...) {
	va_list args;
	int rv;

	__fscanf_file = stream;

	va_start(args, format);
	rv = stdio_scan((const char **)1, format, args);
	va_end(args);

	return rv;
}

int scanf(const char *format, ...) {
	va_list args;
	int rv;

	va_start(args, format);
	rv = stdio_scan(0, format, args);
	va_end(args);

	return rv;
}

void __stdio_unscanchar(const char **str, int ch, int *pc_ptr) {
	if (ch != EOF) {
		(*pc_ptr)--;
	}
	if ((uintptr_t)str >= 2) {
		(*str)--;
	}
	else if ((uintptr_t)str == 1) {
		ungetc(ch, __fscanf_file);
	}
	else {
		ungetc(ch, stdin);
	}
}

int __stdio_scanchar(const char **str, int *pc_ptr) {
	int ch = 0;

	if ((uintptr_t)str >= 2) {
		ch = **str;
		(*str)++;
		ch = (ch == '\0' ? EOF : ch);
	}
	else if ((uintptr_t)str == 1) {
		ch = getc(__fscanf_file);
	}
	else {
		if ('\n' == (ch = getchar())) {
			ch = EOF;
		}
	}

	if (ch != EOF) {
		(*pc_ptr)++;
	}
	return ch;
}
