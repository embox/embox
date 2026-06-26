/**
 * @file
 *
 * @date 15.11.13
 * @author Anton Bondarev
 */

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t nbyte) {
	struct iovec iov;

	iov.iov_base = buf;
	iov.iov_len = nbyte;

	return readv(fd, &iov, 1);
}
