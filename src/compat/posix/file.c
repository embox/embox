/**
 * @file
 *
 * @brief
 *
 * @date 06.11.2011
 * @author Anton Bondarev
 */
#include <fcntl.h>
#include <kernel/task.h>
#include <net/socket.h>
#include <util/array.h>

ARRAY_SPREAD_DEF(struct task_res_ops *, __task_res_ops);

int open(const char *path, int __oflag, ...) {
	return task_res_idx_alloc(task_self_res(), TASK_IDX_TYPE_FILE, fopen(path, "rw"));
}

static struct task_res_ops *find_res_ops(int fd) {
	struct idx_desc *idx_desc = task_self_idx_get(fd);
	int type = task_idx_desc_get_type(idx_desc);

	const struct task_res_ops *res;
	array_foreach(res, __task_res_ops, ARRAY_SPREAD_SIZE(__task_res_ops)) {
		if (res->type == type) {
			return (struct task_res_ops *) res;
		}
	}
	return NULL;
}

int close(int fd) {
	int res = 0;
	struct task_res_ops *ops = find_res_ops(fd);
	struct idx_desc *desc = task_self_idx_get(fd);
	if (task_idx_desc_link_count_add(desc, -1) == 0) {
		res = ops->close(fd);
	}
	task_res_idx_free(task_self_res(), fd);
	return res;
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
	struct task_res_ops *ops = find_res_ops(fd);
	return ops->write(fd, buf, nbyte);
}

ssize_t read(int fd, void *buf, size_t nbyte) {
	struct task_res_ops *ops = find_res_ops(fd);
	return ops->read(fd, buf, nbyte);
}

int ioctl(int fd, int request, va_list args) {
	return fioctl(task_self_idx_get(fd)->file, request, args);
}

int fsync(int fd) {
	return 0;
}

