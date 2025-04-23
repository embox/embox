/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <fcntl.h>

#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc_table.h>
#include <fs/kfile.h>
#include <fs/file_desc.h>

extern const struct idesc_ops idesc_file_ops;

#if 0
static int kftruncate(struct file_desc *desc, off_t length) {
	int ret;
	off_t pos;

	pos = file_get_pos(desc);

	ret = ktruncate(desc->f_inode, length);
	if (0 > ret) {
		/* XXX ktruncate sets errno */
		return -errno;
	}

	if (pos > length) {
		file_set_pos(desc, length);
	}

	return 0;
}
#endif

int ftruncate(int fd, off_t length) {
	struct idesc *idesc;
	struct file_desc *file;
	off_t pos;
	int ret;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))
			|| (idesc->idesc_ops != &idesc_file_ops)) {
		return SET_ERRNO(EBADF);
	}

	if ((idesc->idesc_flags & O_ACCESS_MASK) == O_RDONLY) {
		return SET_ERRNO(EINVAL);
	}

	file = (struct file_desc *) idesc;

	pos = file_get_pos(file);

	ret = ktruncate(file->f_inode, length);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	if (pos > length) {
		file_set_pos(file, length);
	}

	return ret;
}
