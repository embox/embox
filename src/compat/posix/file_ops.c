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

#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>

int close(int fd) {
	const struct task_idx_ops *ops;
	struct idx_desc *desc;
	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ops = task_idx_desc_ops(desc);

	assert(ops);
	assert(ops->close);

	return task_self_idx_table_unbind(fd);
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
	const struct task_idx_ops *ops;
	struct idx_desc *desc;

	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (!buf) {
		SET_ERRNO(EFAULT);
		return -1;
	}

	ops = task_idx_desc_ops(desc);
	assert(ops);
	assert(ops->write);
	return ops->write(desc, buf, nbyte);
}

ssize_t read(int fd, void *buf, size_t nbyte) {
	const struct task_idx_ops *ops;
	struct idx_desc *desc;

	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (!buf) {
		SET_ERRNO(EFAULT);
		return -1;
	}

	ops = task_idx_desc_ops(desc);
	assert(ops);
	assert(ops->read);
	return ops->read(desc, buf, nbyte);
}

int lseek(int fd, long int offset, int origin) {
	const struct task_idx_ops *ops;
	struct idx_desc *desc;

	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ops = task_idx_desc_ops(desc);
	assert(ops);
	assert(ops->fseek);
	return ops->fseek(desc, offset, origin);
}

int ioctl(int fd, int request, ...) {
	va_list args;
	const struct task_idx_ops *ops;
	int ret = -ENOTSUP;
	struct idx_desc *desc;

	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ops = task_idx_desc_ops(desc);

	assert(ops);

	va_start(args, request);

	/* POSIX says, that this way to make read/write nonblocking is old
	 * and recommend use fcntl(fd, O_NONBLOCK, ...)
	 * */
	if (request == FIONBIO && va_arg(args, int) != 0) {
		fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | O_NONBLOCK);
	}

	if (NULL == ops->ioctl) {
		ret = -1;
	} else {
		ret = ops->ioctl(desc, request, args);
	}

	va_end(args);

	return ret;
}

int fstat(int fd, struct stat *buff) {
	const struct task_idx_ops *ops;
	struct idx_desc *desc;

	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ops = task_idx_desc_ops(desc);
	assert(ops);
	if(NULL != ops->fstat) {
		return ops->fstat(desc, buff);
	}
	else {
		return -1;
	}
}

int fcntl(int fd, int cmd, ...) {
	va_list args;
	int res = 0, flag;
	const struct task_idx_ops *ops;
	struct idx_desc *desc;

	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ops = task_idx_desc_ops(desc);

	va_start(args, cmd);

	switch(cmd) {
	case F_GETFD:
		res = *task_idx_desc_flags_ptr(desc);
		return res;
	case F_SETFD:
		flag = va_arg(args, int);
		*task_idx_desc_flags_ptr(desc) = flag;
		if (flag & O_NONBLOCK) {
			io_op_unblock(&desc->data->read_state);
			io_op_unblock(&desc->data->write_state);
		}
		break;
	case F_GETPIPE_SZ:
	case F_SETPIPE_SZ:
		break;
	default:
		/*SET_ERRNO(EINVAL);*/
		res = -1;
		break;
	}

	if (NULL == ops->fcntl) {
		return -1;
	}

	res = ops->fcntl(desc, cmd, args);

	va_end(args);

	return res;
}

int fsync(int fd) {
	return 0;
}
