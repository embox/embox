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

int fgetc(FILE *file) {
	unsigned char ch;
	if (fread(&ch, 1, 1, file) != 1) {
		return EOF;
	}
	return (int)ch;

}

int getc(FILE *f) {
	return fgetc(f);
}

int getchar(void) {
	return getc(stdin);
}

int ungetchar(int ch) {
	return ungetc(ch, stdin);
}
