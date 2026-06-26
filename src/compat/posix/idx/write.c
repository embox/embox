/**
 * @file
 *
 * @date 15.11.13
 * @author Anton Bondarev
 */

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t nbyte) {
	struct iovec iov;

	iov.iov_base = (void *)buf;
	iov.iov_len = nbyte;

	return writev(fd, &iov, 1);
}
