/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "file_struct.h"

int fseek(FILE *file, long int offset, int origin) {
	off_t ret;

	if (origin != SEEK_SET && origin != SEEK_CUR
			&& origin != SEEK_END) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = lseek(file->fd, offset, origin);
	if (ret == (off_t)-1) {
		return -1;
	}

	return 0;
}

int fseeko(FILE *file, off_t offset, int origin) {
	return fseek(file, offset, origin);
}

long int ftell(FILE *file) {
	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}
	return lseek(file->fd, 0L, SEEK_CUR);
}

off_t ftello(FILE *file) {
	return ftell(file);
}

int fgetpos(FILE *stream, fpos_t *pos) {
	off_t mypos;

	if (NULL == stream) {
		SET_ERRNO(EBADF);
		return -1;
	}

	mypos = lseek(stream->fd, 0L, SEEK_CUR);

	if (-1 == mypos) {
		return -1;
	}

	*pos = mypos;

	return 0;
}

int fsetpos(FILE *stream, const fpos_t *pos) {
	off_t ret;

	if (NULL == stream) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = lseek(stream->fd, *pos, SEEK_SET);
	if (ret == (off_t)-1) {
		return -1;
	}

	return 0;
}

void rewind(FILE *file) {
	fseek(file, 0L, SEEK_SET);
}
