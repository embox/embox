/**
 * @file
 *
 * @date Jun 6, 2014
 * @author: Anton Bondarev
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "file_struct.h"

extern void stdio_file_free(FILE *file);

int fclose(FILE *file) {
	int res;

	if (NULL == file){
		SET_ERRNO(EBADF);
		return -1;
	}

	if (funopen_check(file)) {
		/* An error on closefn() does not keep the stream open. */
		if (file->closefn) {
			res = file->closefn((void *) file->cookie);
		} else {
			res = 0;
		}
		stdio_file_free(file);
	} else {
		res = close(file->fd);
		if (res >= 0) {
			stdio_file_free(file);
		}
	}

	return res;
}

