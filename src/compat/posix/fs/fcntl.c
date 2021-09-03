/**
 * @file
 *
 * @date Nov 21, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <fcntl.h>

#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc_table.h>

int fcntl(int fd, int cmd, ...) {
	int ret;
	va_list args;
	union {
		int i;
	} uargs;

	if (!idesc_index_valid(fd)
			|| (NULL == index_descriptor_get(fd))) {
		return SET_ERRNO(EBADF);
	}

	/* Fcntl works in two steps:
	 * 1. Make general commands like F_SETFD, F_GETFD.
	 * 2. If fd has some internal fcntl(), it will be called.
	 *    Otherwise result of point 1 will be returned. */
	switch (cmd) {
	case F_DUPFD:
		va_start(args, cmd);
		uargs.i = va_arg(args, int);
		va_end(args);
		ret = idesc_index_valid(uargs.i)
			? index_descriptor_dupfd(fd, uargs.i)
			: -EBADF;
		if (ret >= 0) {
			/* clean CLOEXEC flag */
			index_descriptor_cloexec_set(ret, 0);
		}
		break;
	case F_DUPFD_CLOEXEC:
		va_start(args, cmd);
		uargs.i = va_arg(args, int);
		va_end(args);
		ret = idesc_index_valid(uargs.i)
			? index_descriptor_dupfd(fd, uargs.i)
			: -EBADF;
		if (ret >= 0) {
			/* set CLOEXEC flag */
			index_descriptor_cloexec_set(ret, FD_CLOEXEC);
		}
		break;
	case F_GETFL:
		return index_descriptor_flags_get(fd);
	case F_SETFL:
		va_start(args, cmd);
		index_descriptor_flags_set(fd, va_arg(args, int));
		va_end(args);
		return 0;
	case F_GETFD: /* only for CLOEXEC flag */
		return index_descritor_cloexec_get(fd);
	case F_SETFD: /* only for CLOEXEC flag */
		va_start(args, cmd);
		index_descriptor_cloexec_set(fd, va_arg(args, int));
		va_end(args);
		return 0;
	default:
		va_start(args, cmd);
		ret = index_descriptor_fcntl(fd, cmd, va_arg(args, void *));
		va_end(args);
		break;
	}

	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return ret;
}

