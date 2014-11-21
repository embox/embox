/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    27.06.2012
 */

#include <stdio.h>
#include <framework/mod/options.h>

int fputc(int c, FILE *stream) {
	char ch = (char) c;

	if (!fwrite(&ch, 1, 1, stream)) {
		return EOF;
	}

	return c;
}

int putc(int c, FILE *f) {
	return fputc(c, f);
}

int putchar(int c) {
	return fputc(c, stdout);
}
