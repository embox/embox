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

ssize_t read(int fd, void *buf, size_t nbyte) {
	ssize_t ret;
	struct iovec iov;
	struct idesc *idesc;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))
			|| ((idesc->idesc_flags & O_ACCESS_MASK) == O_WRONLY)) {
		return SET_ERRNO(EBADF);
	}

	assert(idesc->idesc_ops);
	assert(idesc->idesc_ops->id_readv);

	iov.iov_base = buf;
	iov.iov_len = nbyte;

	ret = idesc->idesc_ops->id_readv(idesc, &iov, 1);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return ret;
}
