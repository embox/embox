/**
 * @file
 *
 * @date Sep 14, 2020
 * @author Anton Bondarev
 */

#include <errno.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>


ssize_t pwrite(int fd, const void *buf, size_t nbyte, off_t offset) {
	int _err;
	ssize_t res;
	off_t old_offset;

	old_offset = lseek (fd, 0, SEEK_CUR);
	if (old_offset == (off_t) -1) {
		return -1;
	}

	if (lseek(fd, offset, SEEK_SET) == (off_t) -1) {
		return -1;
	}

	res = write(fd, buf, nbyte);

	_err = errno;
	if (lseek (fd, old_offset, SEEK_SET) == (off_t) -1) {
		if (res == -1) {
			return SET_ERRNO(_err);
		}
		return -1;
	}

	SET_ERRNO(_err);
	return res;
}
