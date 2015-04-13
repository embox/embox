/**
 * @file
 *
 * @brief
 *
 * @date 08.09.2011
 * @author Anton Bondarev
 */

#include <stdio.h>

int fgetc(FILE *file) {
	unsigned char ch;
	if (fread(&ch, 1, 1, file) != 1) {
		return EOF;
	}
	return (int)ch;

}

int getchar(void) {
	return getc(stdin);
}
