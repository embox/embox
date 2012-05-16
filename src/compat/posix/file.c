/**
 * @file
 *
 * @brief
 *
 * @date 06.11.2011
 * @author Anton Bondarev
 */

#include <errno.h>
#include <fcntl.h>
#include <kernel/task.h>
#include <net/socket.h>
#include <util/array.h>

ARRAY_SPREAD_DEF(struct task_res_ops *, __task_res_ops);

static struct task_res_ops *find_res_ops_by_type(int res_ops_type) {
	const struct task_res_ops *res;
	array_foreach(res, __task_res_ops, ARRAY_SPREAD_SIZE(__task_res_ops)) {
		if (res->type == res_ops_type) {
			return (struct task_res_ops *) res;
		}
	}

	return NULL;
}


static struct task_res_ops *find_res_ops(int fd) {
	struct idx_desc *idx_desc = task_self_idx_get(fd);
	int type = task_idx_desc_get_type(idx_desc);

	return find_res_ops_by_type(type);
}

int open(const char *path, int __oflag, ...) {
	va_list args;
	int ret = -1;
	struct task_res_ops *ops = find_res_ops_by_type(TASK_IDX_TYPE_FILE);

	assert(ops);
	assert(ops->open);

	va_start(args, __oflag);
	ret = ops->open(path, __oflag, args);
	va_end(args);

	return ret;
}

int close(int fd) {
	int res = 0;
	struct task_res_ops *ops = find_res_ops(fd);
	struct idx_desc *desc = task_self_idx_get(fd);

	assert(ops);
	assert(ops->close);

	if (task_idx_desc_link_count_remove(desc)) {
		res = ops->close(fd);
	}

	task_res_idx_free(task_self_res(), fd);

	return res;
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
	struct task_res_ops *ops = find_res_ops(fd);
	assert(ops);
	assert(ops->write);
	return ops->write(fd, buf, nbyte);
}

ssize_t read(int fd, void *buf, size_t nbyte) {
	struct task_res_ops *ops = find_res_ops(fd);
	assert(ops);
	assert(ops->read);
	return ops->read(fd, buf, nbyte);
}

int lseek(int fd, long int offset, int origin) {
	struct task_res_ops *ops = find_res_ops(fd);
	assert(ops);
	assert(ops->fseek);
	return ops->fseek(fd, offset, origin);
}

int ioctl(int fd, int request, ...) {
	va_list args;
	int ret = -ENOTSUP;
	struct task_res_ops *ops = find_res_ops(fd);

	assert(ops);
	assert(ops->ioctl);

	va_start(args, request);
	ret = ops->ioctl(fd, request, args);
	va_end(args);

	return ret;
}

int fsync(int fd) {
	return 0;
}

