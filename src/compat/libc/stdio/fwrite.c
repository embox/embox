/**
 * @file
 *
 * @date Jun 6, 2014
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <util/math.h>
#include <fcntl.h>
#include "file_struct.h"
#include <stdio.h>

static int libc_write(FILE *file, const void *buf, size_t len) {
	int ret;

	if (funopen_check(file)) {
		if (file->writefn) {
			ret = file->writefn((void *) file->cookie, buf, len);
		} else {
			ret = 0;
		}
	} else {
		ret = write(file->fd, buf, len);
	}

	return ret;
}

static int libc_ob_check(FILE *file) {
	return file->obuf != NULL ? 0 : -1;
}

int libc_ob_forceflush(FILE *file) {
	int err;

	if (0 > libc_ob_check(file)) {
		return 0;
	}

	if (0 > (err = libc_write(file, file->obuf, file->obuf_len))) {
		return err;
	}

	file->obuf_len = 0;

	return 0;
}

static int libc_ob_tryflush(FILE *file) {

	if (file->obuf_len == file->obuf_sz) {
		return libc_ob_forceflush(file);
	}

	return 0;
}

static int libc_ob_add(FILE *file, const void *buf, size_t len) {
	const void *cbuf = buf;
	int clen = len;
	int err;

	if (0 > libc_ob_check(file)) {
		return libc_write(file, buf, len);
	}

	/* have in buffer */
	if (file->obuf_len) {
		const int appendlen = min(file->obuf_sz - file->obuf_len, len);

		memcpy(file->obuf + file->obuf_len, cbuf, appendlen);

		file->obuf_len += appendlen;
		cbuf += appendlen;
		clen -= appendlen;

		if (0 > (err = libc_ob_tryflush(file))) {
			return err;
		}
	}
	/* last unwritten */
	if (clen > 0) {
		const int fullob_n = clen / file->obuf_sz;
		int i_fullob;

		for (i_fullob = 0; i_fullob < fullob_n; ++i_fullob) {
			libc_write(file, buf + i_fullob * file->obuf_sz, file->obuf_sz);
		}

		cbuf += fullob_n * file->obuf_sz;
		clen -= fullob_n * file->obuf_sz;

		file->obuf_len = clen;
		memcpy(file->obuf, cbuf, clen);
	}

	return 0;
}

static int libc_ob_line(FILE *file, const void *buf, size_t len) {
	int err;
	const void *nl = memrchr(buf, '\n', len);

	if (nl) {
		const int writelen = nl - buf + 1;
		if (0 > (err = libc_ob_add(file, buf, writelen))) {
			return err;
		}
		if (0 > (err = libc_ob_forceflush(file))) {
			return err;
		}
		err = libc_ob_add(file, buf, len - writelen);
	} else {
		err = libc_ob_add(file, buf, len);
	}

	return err;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	int i_block;

	if (NULL == file) {
		errno = EBADF;
		return 0;
	}

	if ((file->flags & O_ACCESS_MASK) == O_RDONLY) {
		SET_IO_ERR(file);
		return 0;
	}

	for (i_block = 0; i_block < count; ++i_block) {
		const void *block = buf + i_block * size;
		int err;

		if (_IOLBF == file->buftype) {
			err = libc_ob_line(file, block, size);
		} else if (_IOFBF == file->buftype) {
			err = libc_ob_add(file, block, size);
		} else {
			err = libc_write(file, block, size);
		}

		if (0 > err) {
			errno = err;
			return i_block;
		}
	}

	return count;
}
