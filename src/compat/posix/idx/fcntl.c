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

/* Record locks: none here. A file system that keeps them overrides this
 * (fs/dvfs/dvfs_lockf.c). */
__attribute__((weak)) int idesc_fcntl_lock(struct idesc *idesc, int cmd,
    struct flock *fl) {
	return -EINVAL;
}

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
		/* The whole word: index_descriptor_flags_get() masks the access mode
		 * off, and the access mode is what F_GETFL is asked for. */
		return index_descriptor_get(fd)->idesc_flags;
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
	case F_GETLK:
	case F_SETLK:
	case F_SETLKW:
		/* Record locks belong to the file system, not to the file's ioctl. */
		va_start(args, cmd);
		ret = idesc_fcntl_lock(index_descriptor_get(fd), cmd,
		    va_arg(args, struct flock *));
		va_end(args);
		break;
	case F_GETPIPE_SZ:
	case F_SETPIPE_SZ:
		/* The only commands a driver answers (the pipe's) */
		va_start(args, cmd);
		ret = index_descriptor_fcntl(fd, cmd, va_arg(args, void *));
		va_end(args);
		break;
	default:
		/* POSIX: an unknown command is EINVAL, not an ioctl of the file. */
		ret = -EINVAL;
		break;
	}

	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return ret;
}

