/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */

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

	idesc = idesc_common_get(fd);
	if(!idesc) {
		return 0;
	}
	return idesc_close(idesc);

#endif
}
