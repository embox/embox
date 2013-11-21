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

#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>

#include <fs/idesc.h>

static inline int ioctl_inherite(struct idesc *idesc, int request, void *data) {
	assert(idesc->idesc_ops);

	if (!idesc->idesc_ops->ioctl) {
		return SET_ERRNO(ENOTSUP);
	}
	return idesc->idesc_ops->ioctl(idesc, request, data);
}

int ioctl(int fd, int request, ...) {
	void *data;
	struct idesc *idesc;
	va_list args;
	int rc = -ENOTSUP;

	if (!idesc_index_valid(fd)) {
		return SET_ERRNO(EBADF);
	}
	idesc = idesc_common_get(fd);
	if (NULL == idesc) {
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
	case FIONREAD: {
#ifndef IDESC_TABLE_USE
		struct idx_desc *desc;
		assert(task_self_idx_table());

		desc = task_self_idx_get(fd);
		if (!desc) {
			SET_ERRNO(EBADF);
			rc = -1;
			DPRINTF(("EBADF "));
			break
		}

		/* FIXME set actual amount of bytes */
		*va_arg(args, int *) = io_sync_ready(
				task_idx_desc_ios(desc), IO_SYNC_READING);
#else

		//*va_arg(args, int *) = io_sync_ready(idesc, IO_SYNC_READING);
#endif
		break;
	}
	default:
		data = va_arg(args, void*);
		rc = ioctl_inherite(idesc, request, data);
		break;

	}

	va_end(args);

	DPRINTF(("ioctl(%d) = %d\n", fd, rc));
	return rc;
}

static inline int fcntl_inherit(struct idesc *idesc, int cmd, void *data) {
	assert(idesc->idesc_ops);
	if (!idesc->idesc_ops->ioctl) {
		return SET_ERRNO(ENOTSUP);
	}
	return idesc->idesc_ops->ioctl(idesc, cmd, data);
}

int fcntl(int fd, int cmd, ...) {
	struct idesc *idesc;
	void * data;
	va_list args;

	if (!idesc_index_valid(fd)) {
		return SET_ERRNO(EBADF);
	}

	idesc = idesc_common_get(fd);
	if (NULL == idesc) {
		return SET_ERRNO(EBADF);
	}

	/* Fcntl works in two steps:
	 * 1. Make general commands like F_SETFD, F_GETFD.
	 * 2. If fd has some internal fcntl(), it will be called.
	 *    Otherwise result of point 1 will be returned. */
	switch (cmd) {
	case F_GETFD:
		return idesc->idesc_flags;
	case F_SETFD:
		va_start(args, cmd);
		idesc->idesc_flags = va_arg(args, int);
		va_end(args);
		return 0;
/*	case F_GETPIPE_SZ:
	case F_SETPIPE_SZ:
		break;
*/
	default:
		va_start(args, cmd);
		data = va_arg(args, void*);
		va_end(args);
		return fcntl_inherit(idesc, cmd, data);
	}

	return SET_ERRNO(ENOTSUP);
}

