/**
 * @file
 *
 * @brief
 *
 * @date 08.09.2011
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <kernel/file.h>
#include <ctype.h>

int fgetc(FILE *file) {
	unsigned char ch;
	fread(&ch, 1, 1, file);
	return (int)ch;

}

int getchar(void) {
	int c;
	c = getc(stdin);
	if (isalnum(c) || isspace(c)) {
		putchar(c);
	}
	return c;
}

int ungetchar(int ch) {
	return ungetc(ch, stdin);
}
