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

int __file_opened_fd(int fd, FILE *file, struct task_resources *res) {
	struct __fd_list *fdl = &res->fds[fd];

	assert(fdl->file == NULL);

	fdl->file = file;

	return fd;
}

int task_file_open(FILE *file, struct task_resources *res) {
	struct __fd_list *fdl = task_fdl_alloc(res);
	fdl->file = file;
	return task_desc2idx(fdl, res);
}

static int attempt_real_close(struct __fd_list *fdl) {
	if (list_empty(&fdl->link)) {
		fclose(fdl->file);
		return 1;
	}
	return 0;
}

int task_file_close(int fd, struct task_resources *res) {
	struct __fd_list *fdl = &res->fds[fd];
	FILE *file = fdl->file;

	if (file == NULL) {
		return -EBADF;
	}

	attempt_real_close(fdl);

	fdl->file = NULL;
	list_move(&fdl->link, &res->fds_free);

	return ENOERR;
}

int task_file_reopen(int fd, FILE *file){
	struct __fd_list *fdl = &task_self()->resources.fds[fd];

	attempt_real_close(fdl);

	fdl->file = file;

	return 0;
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

