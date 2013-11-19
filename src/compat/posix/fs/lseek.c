/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>

#include <fs/kfile.h>
#include <fs/file_desc.h>


extern const struct task_idx_ops task_idx_ops_file;
off_t lseek(int fd, off_t offset, int origin) {
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
	assert(ops->fseek);
	return ops->fseek(desc, offset, origin);
#else
	struct file_desc *desc;
	desc = file_desc_get(fd);
	assert(desc);

	if (desc->idesc.idesc_ops != &task_idx_ops_file) {
		return -EBADF;
	}

	return kseek(desc, offset, origin);
#endif

}
