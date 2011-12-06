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

#include "index_desc.h"

extern const struct task_res_ops * __task_res_ops[];

static int this_close(int fd) {
	FILE *file = task_self_idx_get(fd)->file;
	return fclose(file);
}

static ssize_t this_read(int fd, const void *buf, size_t nbyte) {
	FILE *file = task_self_idx_get(fd)->file;
	return fread((void *) buf, 1, nbyte, file);
}

static ssize_t this_write(int fd, const void *buf, size_t nbyte) {
	FILE *file = task_self_idx_get(fd)->file;
	return fwrite(buf, 1, nbyte, file);
}

static struct task_res_ops ops = {
	.type = TASK_IDX_TYPE_FILE,
	.read = this_read,
	.write = this_write,
	.close = this_close
};

ARRAY_SPREAD_ADD(__task_res_ops, &ops);
