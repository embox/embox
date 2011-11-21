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

int __file_opened_fd(int fd, FILE *file, struct task_resources *res) {
	struct __fd_list *fdl = &res->fds[fd];

	assert(fdl->file == NULL);

	fdl->file = file;
	list_move(&fdl->link, &res->fds_opened);

	//fdl->unchar = EOF;
	return fd;
}

int task_file_open(FILE *file, struct task_resources *res) {
	int fd = task_idx_alloc(TASK_IDX_TYPE_FILE);
	return __file_opened_fd(fd, file, res);
}

int task_file_close(int fd, struct task_resources *res) {
	struct __fd_list *fdl = &res->fds[fd];
	FILE *file = fdl->file;

	if (file == NULL) {
		return -EBADF;
	}

	if (list_empty(&fdl->link)) {
		fclose(file);
	}

	fdl->file = NULL;

	list_move(&fdl->link, &res->fds_free);

	return ENOERR;
}

int task_file_reopen(int fd, FILE *file){
	close(fd);

	return __file_opened_fd(fd, file, &task_self()->resources);
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
	return fwrite(buf, 1, nbyte, task_self()->resources.fds[fd].file);
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

