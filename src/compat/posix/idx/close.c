/**
 * @file
 *
 * @date 15.11.13
 * @author Anton Bondarev
 */

#include <errno.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc.h>

/* Drops the task's record locks on the file. A file system that keeps them
 * overrides this (fs/dvfs/dvfs_lockf.c). */
__attribute__((weak)) void idesc_close_locks(struct idesc *idesc) {
}

int close(int fd) {
	int ret;
	struct idesc *idesc;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))) {
		return SET_ERRNO(EBADF);
	}

	/* POSIX: closing any descriptor of a file drops every record lock the
	 * process holds on it -- this one need not be the one that took them. */
	idesc_close_locks(idesc);

	ret = idesc_close(idesc, fd);
	if (ret != 0) {
		return SET_ERRNO(-ret);
	}

	return 0;
}
