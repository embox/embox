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

//XXX should be just close, interference with socket's close
int close(int fd) {
	switch(task_idx_to_type(fd)) {
	case TASK_IDX_TYPE_FILE:
		return task_file_close(fd, task_get_resources(task_self()));
	case TASK_IDX_TYPE_SOCKET:
//		return socket_close(fd);
	default:
		return -1;
	}
}
