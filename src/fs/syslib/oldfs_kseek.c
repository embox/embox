/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <fs/inode.h>
#include <fs/file_desc.h>

int kseek(struct file_desc *desc, long int offset, int origin) {
	off_t pos;

	pos = file_get_pos(desc);

	if (NULL == desc) {
		return -EBADF;
	}

	switch (origin) {
		case SEEK_SET:
			pos = offset;
			break;

		case SEEK_CUR:
			pos += offset;
			break;

		case SEEK_END:
			pos = desc->f_inode->i_size + offset;
			break;

		default:
			return -EINVAL;
	}

	file_set_pos(desc, pos);
	return pos;
}
