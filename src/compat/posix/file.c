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
#include <kernel/task.h>
#include <net/socket.h>
#include <util/array.h>
#include <fs/posix.h>

int open(const char *path, int __oflag, ...) {
	char mode;

	if ((O_RDWR == __oflag) || (O_WRONLY == __oflag)) {
		mode = 'w';
	}
	else {
		mode = 'r';
	}

	return task_res_idx_alloc(task_self_res(), &task_res_ops_file, \
			fopen(path, (const char *) &mode));
}

int close(int fd) {
	int res = 0;
	struct idx_desc *desc = task_self_idx_get(fd);
	const struct task_res_ops *ops = task_idx_desc_ops(desc);

	assert(ops);
	assert(ops->close);

	if (task_idx_desc_link_count_remove(desc)) {
		res = ops->close(task_idx_desc_data(desc));
	}

	task_res_idx_free(task_self_res(), fd);

	return res;
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
	struct idx_desc *desc = task_self_idx_get(fd);
	const struct task_res_ops *ops = task_idx_desc_ops(desc);
	assert(ops);
	assert(ops->write);
	return ops->write(task_idx_desc_data(desc), buf, nbyte);
}

ssize_t read(int fd, void *buf, size_t nbyte) {
	struct idx_desc *desc = task_self_idx_get(fd);
	const struct task_res_ops *ops = task_idx_desc_ops(desc);
	assert(ops);
	assert(ops->read);
	return ops->read(task_idx_desc_data(desc), buf, nbyte);
}

int lseek(int fd, long int offset, int origin) {
	struct idx_desc *desc = task_self_idx_get(fd);
	const struct task_res_ops *ops = task_idx_desc_ops(desc);
	assert(ops);
	assert(ops->fseek);
	return ops->fseek(task_idx_desc_data(desc), offset, origin);
}

int ioctl(int fd, int request, ...) {
	va_list args;
	int ret = -ENOTSUP;
	struct idx_desc *desc = task_self_idx_get(fd);
	const struct task_res_ops *ops = task_idx_desc_ops(desc);

	assert(ops);
	assert(ops->ioctl);

	va_start(args, request);
	ret = ops->ioctl(task_idx_desc_data(desc), request, args);
	va_end(args);

	return ret;
}

int fsync(int fd) {
	return 0;
}

