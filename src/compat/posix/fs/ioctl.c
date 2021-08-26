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

#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc_table.h>

/* POSIX says, that this way to make read/write nonblocking is old
 * and recommend use fcntl(fd, O_NONBLOCK, ...)
 * */
static int io_fionbio(int fd, va_list args) {
	const int *val_p = va_arg(args, const int *);
	int flags;

	/* if fcntl returns -1, then errno already set, can just return -1 */

	flags = fcntl(fd, F_GETFL);
	if (-1 == flags) {
		return -1;
	}

	if (*val_p != 0) {
		flags |= O_NONBLOCK;
	} else {
		flags &= ~O_NONBLOCK;
	}

	if (-1 == fcntl(fd, F_SETFL, flags)) {
		return -1;
	}

	return 0;
}

int ioctl(int fd, int request, ...) {
	void *data;
	va_list args;
	int rc;

	if (!idesc_index_valid(fd)) {
		return SET_ERRNO(EBADF);
	}

	switch (request) {
  	case FIONBIO:
		va_start(args, request);
		rc = io_fionbio(fd, args);
		va_end(args);
		break;
	case FIONREAD:
		rc = index_descriptor_status(fd, POLLIN);
		if (rc < 0) {
			rc = SET_ERRNO(-rc);
		}
		break;
	default:
		va_start(args, request);
		data = va_arg(args, void*);
		va_end(args);

		rc = index_descriptor_ioctl(fd, request, data);
		if (rc < 0) {
			rc = SET_ERRNO(-rc);
		}
		break;
	}

	return rc;
}



