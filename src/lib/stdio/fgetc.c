/**
 * @file
 *
 * @brief
 *
 * @date 08.09.2011
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <fs/file_operation.h>
#include <ctype.h>

int fgetc(FILE *file) {
	unsigned char ch;
	if (fread(&ch, 1, 1, file) != 1) {
		return EOF;
	}
	return (int)ch;

}

int getchar(void) {
	int c;
	c = getc(stdin);
	if (!iscntrl(c) || isspace(c)) {
		putchar(c);
	}
	return c;
}

int ungetchar(int ch) {
	return ungetc(ch, stdin);
}
