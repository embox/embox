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
#include <util/array.h>
#include "task_common.h"

extern int task_desc2idx(struct __fd_list *desc, struct task_resources *res);

extern const struct task_res_ops * __task_res_ops[];

static int this_close(int fd) {
	return task_file_close(fd, task_get_resources(task_self()));
}

static ssize_t this_read(int fd, const void *buf, size_t nbyte) {
	return fread((void *) buf, 1, nbyte, task_get_resources(task_self())->fds[fd].file);
}

static ssize_t this_write(int fd, const void *buf, size_t nbyte) {
	return fwrite(buf, 1, nbyte, (void *) task_get_resources(task_self())->fds[fd].file);
}

static struct task_res_ops ops = {
	.type = TASK_IDX_TYPE_FILE,
	.read = this_read,
	.write = this_write,
	.close = this_close
};

ARRAY_SPREAD_ADD(__task_res_ops, &ops);

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

