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

int fgetc(FILE *file) {
	unsigned char ch;
	fread(&ch, 1, 1, file);
	return (int)ch;

}

int getchar(void) {
	return getc(stdin);
}

int ungetchar(int ch) {
	return ungetc(ch, stdin);
}
