/**
 * @file
 *
 * @brief
 *
 * @date 06.11.2011
 * @author Anton Bondarev
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <poll.h>


#include <fs/index_descriptor.h>
#include <kernel/task/idesc_table.h>


int ioctl(int fd, int request, ...) {
	void *data;
	va_list args;
	int rc = -ENOTSUP;

	if (!idesc_index_valid(fd)) {
		return SET_ERRNO(EBADF);
	}

	va_start(args, request);

	switch (request) {
  	case FIONBIO:
		/* POSIX says, that this way to make read/write nonblocking is old
		 * and recommend use fcntl(fd, O_NONBLOCK, ...)
		 * */
		if (va_arg(args, int) != 0) {
			fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | O_NONBLOCK);
		}
		break;
	case FIONREAD:
		rc = index_descriptor_status(fd, POLLIN);
		if (rc < 0) {
			SET_ERRNO(-rc);
		}
		break;
	default:
		data = va_arg(args, void*);
		rc = index_descriptor_ioctl(fd, request, data);
		if (rc < 0) {
			SET_ERRNO(-rc);
		}
		break;
	}

	va_end(args);

	return rc;
}



