/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */
#include <stddef.h>
#include <unistd.h>

#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>

#include <fs/idesc.h>

ssize_t write(int fd, const void *buf, size_t nbyte) {
	const struct task_idx_ops *ops;
#ifndef IDESC_TABLE_USE
	struct idx_desc *desc;

	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (!buf) {
		SET_ERRNO(EFAULT);
		return -1;
	}

	ops = task_idx_desc_ops(desc);
	assert(ops);
	assert(ops->write);
	return ops->write(desc, buf, nbyte);
#else
	struct idesc *desc;
	desc = idesc_common_get(fd);
	assert(desc);

	ops = desc->idesc_ops;

	return ops->write(desc, buf, nbyte);
#endif

}
