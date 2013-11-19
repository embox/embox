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

static inline int ioctl_inherite(int fd, int request, void *data) {
	int rc;
	const struct task_idx_ops *ops;

#ifndef IDESC_TABLE_USE
	struct idx_desc *desc;
	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		SET_ERRNO(EBADF);
		rc = -1;
		DPRINTF(("EBADF "));
		goto end;
	}

	ops = task_idx_desc_ops(desc);
#else
	struct idesc *idesc;
	idesc = idesc_common_get(fd);
	assert(idesc);
	ops = idesc->idesc_ops;
#endif

	assert(ops);

	if (NULL == ops->ioctl) {
		rc = -1;
	} else {
		rc = ops->ioctl(idesc, request, data);
	}
	return rc;

}

int ioctl(int fd, int request, ...) {
	void *data;
	va_list args;
	int rc = -ENOTSUP;


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
		idesc_common_get(fd);

#endif
		break;
	}
	default:
		data = va_arg(args, void*);
		rc = ioctl_inherite(fd,request, data);
		break;

	}

	va_end(args);

	DPRINTF(("ioctl(%d) = %d\n", fd, rc));
	return rc;
}


/* XXX -- whole function seems to be covered by many workarounds
 * try blame it -- Anton Kozlov */
int fcntl(int fd, int cmd, ...) {
#ifndef IDESC_TABLE_USE
	va_list args;
	int res, err, flag;
	const struct task_idx_ops *ops;
	struct idx_desc *desc;

	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		DPRINTF(("EBADF "));
		err = -EBADF;
		res = -1;
		goto end;
	}

	flag = 0;
	ops = task_idx_desc_ops(desc);
	assert(ops != NULL);

	va_start(args, cmd);

	err = -EINVAL;
	res = -1;

	/* Fcntl works in two steps:
	 * 1. Make general commands like F_SETFD, F_GETFD.
	 * 2. If fd has some internal fcntl(), it will be called.
	 *    Otherwise result of point 1 will be returned. */
	switch(cmd) {
	case F_GETFD:
		res = *task_idx_desc_flags_ptr(desc);
		err = 0;
		break;
	case F_SETFD:
		flag = va_arg(args, int);
		*task_idx_desc_flags_ptr(desc) = flag;
		res = 0;
		err = 0;
		break;
	case F_GETPIPE_SZ:
	case F_SETPIPE_SZ:
		err = -ENOTSUP;
		res = -1;
		break;
	default:
		err = -EINVAL;
		res = -1;
		break;
	}

	if (ops->ioctl != NULL) {
		res = ops->ioctl(desc, cmd, (void *)flag);
	}

	va_end(args);
end:
	DPRINTF(("fcntl(%d, %d) = %d\n", fd, cmd, res));
	SET_ERRNO(-err);
	return res;
#else
	return -ENOTSUP;
#endif
}

