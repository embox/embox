/**
 * @file
 * @brief File descriptor (fd) abstraction over FILE *
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */
#include <fs/file_desc.h>
#include <types.h>
#include <errno.h>
#include <kernel/task.h>
#include <lib/list.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <stdio.h>
#include <kernel/file.h>
#include "task_common.h"

extern int task_desc2idx(struct __fd_list *desc, struct task_resources *res);

int task_file_open(FILE *file, struct task_resources *res) {
	int fd = task_idx_alloc(TASK_IDX_TYPE_FILE);
	task_idx_save_res(fd, file, res);
	return fd;
}

static int attempt_real_close(struct __fd_list *fdl) {
	if (list_empty(&fdl->link)) {
		fclose(fdl->file);
		return 1;
	}
	list_del_init(&fdl->link);
	return 0;
}

int task_file_close(int fd, struct task_resources *res) {
	struct __fd_list *fdl = &res->fds[fd];

	if (fdl->file == NULL) {
		return -EBADF;
	}

	attempt_real_close(fdl);

	fdl->file = NULL;

	return ENOERR;
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
	return fwrite(buf, 1, nbyte, task_get_resources(task_self())->fds[fd].file);
}

//TODO doesn't handle unchar
ssize_t read(int fd, void *buf, size_t nbyte) {
	return fread(buf, 1, nbyte, task_self()->resources.fds[fd].file);
}

int ioctl(int fd, int request, va_list args) {
	return fioctl(task_self()->resources.fds[fd].file, request, args);
}

int fsync(int fd) {
	return 0;
}

