/**
 * @file
 *
 * @date 15.11.13
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>

#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_table.h>

ssize_t write(int fd, const void *buf, size_t nbyte) {
	ssize_t ret;
	struct idesc *idesc;
	struct iovec iov;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))
			|| ((idesc->idesc_flags & O_ACCESS_MASK) == O_RDONLY)) {
		return SET_ERRNO(EBADF);
	}

	iov.iov_base = (void *)buf;
	iov.iov_len = nbyte;

	assert(idesc->idesc_ops != NULL);
	ret = idesc->idesc_ops->id_writev(idesc, &iov, 1);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return ret;
}
