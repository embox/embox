/**
 * @file
 *
 * @date Jun 6, 2014
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include "file_struct.h"

#include <stdio.h>

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	char *cbuff;
	size_t cnt;

	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	cnt = 0;
	if (file->has_ungetc) {
		file->has_ungetc = 0;
		cbuff = buf;
		cbuff[0] = (char)file->ungetc;
		count --;
		buf = &cbuff[1];
		cnt++;
	}
	while (cnt != count * size) {
		int ret;
		if (funopen_check(file)) {
			if (file->readfn) {
				ret = file->readfn((void *) file->cookie, buf, size * count);
			} else {
				ret = 0;
			}
		} else {
			ret = read(file->fd,  buf, size * count);
		}
		if (ret <= 0) {
			break; /* errors */
		}
		cnt += ret;
	}
	if (cnt % size) {
		/* try to revert some bytes */
		cnt -= (cnt % size);
	}

	return cnt / size;
}
