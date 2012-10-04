/**
 * @file
 *
 * @brief
 *
 * @date 06.11.2011
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <kernel/task/idx.h>
#include <net/socket.h>
#include <util/array.h>
#include <fs/posix.h>
#include <fs/core.h>

int open(const char *path, int __oflag, ...) {
	char mode[] = "-";

	if ((O_RDWR == __oflag) || (O_WRONLY == __oflag)) {
		mode[0] = 'w';
	}
	else {
		mode[0] = 'r';
	}

	return task_self_idx_alloc(&task_idx_ops_file, \
			kopen(path, (const char *)&mode[0]));
}

int close(int fd) {
	const struct task_idx_ops *ops;
	struct idx_desc *desc = task_self_idx_get(fd);

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
	struct idx_desc *desc = task_self_idx_get(fd);

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
	struct idx_desc *desc = task_self_idx_get(fd);

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
	struct idx_desc *desc = task_self_idx_get(fd);

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
	struct idx_desc *desc = task_self_idx_get(fd);

	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ops = task_idx_desc_ops(desc);

	assert(ops);

	va_start(args, request);

	if (NULL == ops->ioctl) {
		ret = -1;
	} else {
		ret = ops->ioctl(desc, request, args);
	}

	va_end(args);

	return ret;
}

int fcntl(int fd, int cmd, ...) {
	va_list args;
	int res;
	struct idx_desc *desc = task_self_idx_get(fd);

	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	va_start(args, cmd);
	switch(cmd) {
	case F_GETFD:
		res = *task_idx_desc_flags_ptr(desc);
		break;
	case F_SETFD:
		* task_idx_desc_flags_ptr(desc) = va_arg(args, int);
		break;
	default:
		/*SET_ERRNO(EINVAL);*/
		res = -1;
		break;
	}
	/* ops->foctl(desc, cmd, args); */
	va_end(args);

	return res;
}

int fsync(int fd) {
	return 0;
}
