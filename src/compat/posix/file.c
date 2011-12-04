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
	return task_file_open(fopen(path, "rw"), task_get_resources(task_self()));
}

int close(int fd) {
	int type = task_idx_to_type(fd);
	const struct task_res_ops *res;
	array_foreach(res, __task_res_ops, ARRAY_SPREAD_SIZE(__task_res_ops)) {
		if (res->type == type) {
			return res->close(fd);
		}
	}
	return -1;
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
	int type = task_idx_to_type(fd);
	const struct task_res_ops *res;
	array_foreach(res, __task_res_ops, ARRAY_SPREAD_SIZE(__task_res_ops)) {
		if (res->type == type) {
			return res->write(fd, buf, nbyte);
		}
	}
	return -1;
}

ssize_t read(int fd, void *buf, size_t nbyte) {
	int type = task_idx_to_type(fd);
	const struct task_res_ops *res;
	array_foreach(res, __task_res_ops, ARRAY_SPREAD_SIZE(__task_res_ops)) {
		if (res->type == type) {
			return res->read(fd, buf, nbyte);
		}
	}
	return -1;
}

int ioctl(int fd, int request, va_list args) {
	return fioctl(task_self()->resources.fds[fd].file, request, args);
}

int fsync(int fd) {
	return 0;
}

