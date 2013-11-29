/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */

#include <stddef.h>
#include <unistd.h>
#include <errno.h>

#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>

#include <fs/index_descriptor.h>

#include <fs/idesc.h>

int ftruncate(int fd, off_t length) {
#ifndef IDESC_TABLE_USE
	const struct task_idx_ops *ops;

	struct idx_desc *desc;

	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);

	if (!desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ops = task_idx_desc_ops(desc);
	assert(ops);

	if (!ops->ftruncate) {
		SET_ERRNO(EBADF);
		return -1;
	}

	return ops->ftruncate(desc, length);
#else
	struct idesc *desc;
	desc = index_descriptor_get(fd);
	assert(desc);

//	ops = desc->idesc_ops;

	return 0;// return ops->ftruncate(desc, length);
#endif

}
