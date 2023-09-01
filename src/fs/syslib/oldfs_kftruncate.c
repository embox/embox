/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <sys/types.h>

#include <fs/inode.h>
#include <fs/file_desc.h>
#include <fs/kfile.h>

int kftruncate(struct file_desc *desc, off_t length) {
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
