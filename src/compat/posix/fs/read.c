/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */


ssize_t read(int fd, void *buf, size_t nbyte) {
	const struct task_idx_ops *ops;
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
	assert(ops->read);
	return ops->read(desc, buf, nbyte);
}
