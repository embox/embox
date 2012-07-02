/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    27.06.2012
 */

#include <stdio.h>

int fputc(int c, FILE *stream) {
	char ch = (char) c;

	if (!fwrite(&ch, 1, 1, stream)) {
		return EOF;
	}

	return c;
}

