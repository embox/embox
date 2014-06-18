/**
 * @file
 *
 * @date Jun 6, 2014
 * @author: Anton Bondarev
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

extern void stdio_file_free(FILE *file);

int fclose(FILE *file) {
	int res;

	if (NULL == file){
		SET_ERRNO(EBADF);
		return -1;
	}

	res = close(file->fd);
	if (res >= 0) {
		stdio_file_free(file);
	}

	return res;
}

