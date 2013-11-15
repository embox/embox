/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int origin) {
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
}
