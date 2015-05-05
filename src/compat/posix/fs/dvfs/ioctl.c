/**
 * @file
 *
 * @brief
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

/* POSIX says, that this way to make read/write nonblocking is old
 * and recommend use fcntl(fd, O_NONBLOCK, ...)
 * */
int io_fionbio(int fd, va_list args) {
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
	return 0;
}

