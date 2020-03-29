/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.11.2014
 */

#include <errno.h>
#include "file_struct.h"

#include <stdio.h>

int ungetc(int ch, FILE *file) {
	if (NULL == file) {
		SET_ERRNO(EBADF);
		return EOF;
	}
	if (EOF != ch) {
		file->ungetc = (char) ch;
		file->has_ungetc = 1;
		file->flags &= ~IO_EOF_;
	}
	return ch;
}

