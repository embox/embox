/**
 * @file
 * @brief File descriptor (fd) abstraction over FILE *
 * @details Provide POSIX kernel support to operate with flides (int)
 *	instead of FILE *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#include <assert.h>
#include <errno.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <fs/flags.h>
#include <fs/kfile.h>

static inline struct file_desc *from_data(struct idx_desc *data) {
	return (struct file_desc *) task_idx_desc_data(data);
}

static int this_close(struct idx_desc *data) {
	return kclose(from_data(data));
}

static ssize_t this_read(struct idx_desc *data, void *buf, size_t nbyte) {
	return kread(buf, nbyte, from_data(data));
}

static ssize_t this_write(struct idx_desc *data, const void *buf, size_t nbyte) {
	assert(data);
	return kwrite(buf, nbyte, from_data(data));
}

static int this_lseek(struct idx_desc *data, long int offset, int origin) {
	return kseek(from_data(data), offset, origin);
}

static int this_stat(struct idx_desc *data, void *buff) {
	return kfstat(from_data(data), buff);
}

static int this_ioctl(struct idx_desc *desc, int request, void *data) {
	return kioctl(from_data(desc), request, data);
}

static int this_truncate(struct idx_desc *data, off_t length) {
	if (!(from_data(data)->flags & FS_MAY_WRITE)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	return ktruncate(from_data(data)->node, length);
}

const struct task_idx_ops task_idx_ops_file = {
	.close = this_close,
	.read  = this_read,
	.write = this_write,
	.fseek = this_lseek,
	.ioctl = this_ioctl,
	.fstat = this_stat,
	.ftruncate = this_truncate
};
