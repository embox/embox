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


int __file_opened_fd(int fd, FILE *file, struct task *tsk) {
	struct __fd_list *fdl = &tsk->fd_array.fds[fd];

	assert(fdl->file == NULL);

	fdl->file = file;
	INIT_LIST_HEAD(&fdl->file_link);
	list_move(&fdl->link, &tsk->fd_array.opened_fds);

	fdl->unchar = EOF;
	return fd;
}

static int file_opened(FILE *file, struct task *tsk) {
	int fd = (int)
			((int) list_entry(tsk->fd_array.free_fds.next, struct __fd_list, link)
			- (int) &tsk->fd_array) / sizeof(struct __fd_list);
	return __file_opened_fd(fd, file, tsk);
}

static void file_closed(int fd, struct task *tsk) {
	tsk->fd_array.fds[fd].file = NULL;
	list_move(&tsk->fd_array.fds[fd].link, &tsk->fd_array.free_fds);
}

int reopen(int fd, FILE *file) {
	int res;

	file_close(fd);

	if (0 != (res = __file_opened_fd(fd, file, task_self()))) {
		return res;
	}

	return 0;
}

int open(const char *path, int __oflag, ...) {
	return file_opened(fopen(path, "rw"), task_self());
}

//XXX should be just close, interference with socket's close
int file_close(int fd) {
	struct __fd_list *fdl = &task_self()->fd_array.fds[fd];
	FILE *file = fdl->file;

	if (file == NULL) {
		return -EBADF;
	}

	if (fdl->file_link.next == &fdl->file_link) {
		fclose(file);
	} else {
		list_del(&fdl->file_link);
	}

	file_closed(fd, task_self());

	return ENOERR;
}

ssize_t write(int fd, const void *buf, size_t nbyte) {
	return fwrite(buf, 1, nbyte, task_self()->fd_array.fds[fd].file);
}

//TODO doesn't handle unchar
ssize_t read(int fd, void *buf, size_t nbyte) {
	return fread(buf, 1, nbyte, task_self()->fd_array.fds[fd].file);
}

int ioctl(int fd, int request, va_list args) {
	return fioctl(task_self()->fd_array.fds[fd].file, request, args);
}

int fsync(int fd) {
	return 0;
}

