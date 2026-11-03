/**
 * @file
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <sys/types.h>

#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc.h>
#include <fs/dvfs.h>

int ftruncate(int fd, off_t length) {
	struct idesc *idesc;
	struct file_desc *file;
	int ret;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))) {
		return SET_ERRNO(EBADF);
	}

	file = (struct file_desc *) idesc;

	if (length < 0) {
		return SET_ERRNO(EINVAL);
	}

	if ((ret = dvfs_file_valid(file))) {
		return SET_ERRNO(-ret);
	}

	if (!file->f_inode || !file->f_inode->i_ops
	    || !file->f_inode->i_ops->ino_truncate) {
		return SET_ERRNO(EINVAL);
	}

	ret = file->f_inode->i_ops->ino_truncate(file->f_inode, length);
	if (ret != 0) {
		return SET_ERRNO(ret < 0 ? -ret : EIO);
	}

	file->f_inode->i_size = length;

	return 0;
}
