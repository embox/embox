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

int open(const char *path, int __oflag, ...) {
	return task_file_open(fopen(path, "rw"), task_get_resources(task_self()));
}

int close(int fd) {
	switch(task_idx_to_type(fd)) {
	case TASK_IDX_TYPE_FILE:
		return task_file_close(fd, task_get_resources(task_self()));
	case TASK_IDX_TYPE_SOCKET:
		return socket_close(fd);
	default:
		return -1;
	}
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
	void *op = task_get_resources(task_self())->fds[fd].file;

	switch(task_idx_to_type(fd)) {
	case TASK_IDX_TYPE_FILE:
		return fwrite(buf, 1, nbyte, op);
	case TASK_IDX_TYPE_SOCKET:
		return sendto(fd, buf, nbyte, 0, NULL, 0);
	default:
		return -1;
	}
}

ssize_t read(int fd, void *buf, size_t nbyte) {
	void *op = task_get_resources(task_self())->fds[fd].file;
	switch(task_idx_to_type(fd)) {
	case TASK_IDX_TYPE_FILE:
		return fread(buf, 1, nbyte, op);
	case TASK_IDX_TYPE_SOCKET:
		return recvfrom(fd, buf, nbyte, 0, NULL, 0);
	default:
		return -1;
	}
}

int ioctl(int fd, int request, va_list args) {
	return fioctl(task_self()->resources.fds[fd].file, request, args);
}

int fsync(int fd) {
	return 0;
}

