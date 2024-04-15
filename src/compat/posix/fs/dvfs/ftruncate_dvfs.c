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

	assert(file->f_inode);
	assert(file->f_inode->i_ops);

	if (!file->f_inode->i_ops->ino_truncate)
		return -EPERM;

	ret = file->f_inode->i_ops->ino_truncate(file->f_inode, length);

	if (ret == 0)
		file->f_inode->i_size = length;

	return ret;
}
