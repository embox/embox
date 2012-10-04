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

int putchar(int c) {
#if OPTION_GET(BOOLEAN,stdio_lf_crlf_map)
	if (c == '\n') {
		fputc('\r', stdout);
	}
#endif
	return fputc(c, stdout);
}
