/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <fs/index_descriptor.h>
#include <fs/kfile.h>
#include <kernel/task/idesc_table.h>

extern const struct idesc_ops idesc_file_ops;

int ftruncate(int fd, off_t length) {
	struct idesc *idesc;
	int ret;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))
			|| (idesc->idesc_ops != &idesc_file_ops)) {
		return SET_ERRNO(EBADF);
	}

	if (!(idesc->idesc_amode & FS_MAY_READ)) {
		return SET_ERRNO(EINVAL);
	}

	ret = kftruncate(member_cast_out(idesc, struct file_desc, idesc), length);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}
	return ret;
}
