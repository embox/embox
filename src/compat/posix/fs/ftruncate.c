/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */


int ftruncate(int fd, off_t length) {
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
}
