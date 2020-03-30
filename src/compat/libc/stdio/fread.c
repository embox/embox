/**
 * @file
 *
 * @date Jun 6, 2014
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include "file_struct.h"

#include <stdio.h>

static size_t __fread(FILE *file, void *buf, size_t len) {
	size_t cnt;

	if (funopen_check(file)) {
		cnt = (file->readfn) ? file->readfn((void *) file->cookie, buf, len) : 0;
	} else {
		cnt = read(file->fd,  buf, len);
	}
	if (cnt <= 0) {
		file->flags |= cnt ? IO_ERR_ : IO_EOF_;
		return 0;
	}
	return cnt;
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	char *cbuff = buf;
	size_t len = size * count, l = len, ret;

	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if ((file->flags & O_ACCESS_MASK) == O_WRONLY) {
		file->flags |= IO_ERR_;
		return 0;
	}

	if (file->has_ungetc) {
		file->has_ungetc = 0;
		*cbuff++ = (char)file->ungetc;
		l--;
	}
	for (; l; l -= ret, cbuff += ret ) {
		ret = (file->flags & IO_EOF_) ? 0 : __fread(file, cbuff, l);
		if (!ret) {
			return (len - l) / size;
		}
	}

	return count;
}
