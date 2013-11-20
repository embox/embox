/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>

#include <kernel/task.h>
#include <kernel/task/idesc_table.h>
#include <fs/idesc.h>

int close(int fd) {
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
	assert(ops->close);

	return task_self_idx_table_unbind(fd);
#else
	struct idesc *idesc;

	if(!idesc_index_valid(fd)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	idesc = idesc_common_get(fd);
	if(!idesc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	return idesc_close(idesc, fd);
#endif
}
