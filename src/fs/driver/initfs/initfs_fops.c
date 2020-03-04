/**
 * @
 *
 * @date Dec 23, 2019
 * @author Anton Bondarev
 */
#include <stddef.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include <fs/file_operation.h>
#include <fs/file_desc.h>

#include "initfs.h"

static size_t initfs_read(struct file_desc *desc, void *buf, size_t size) {
	struct initfs_file_info *fi;
	off_t pos;

	pos = file_get_pos(desc);
	fi = file_get_inode_data(desc);

	if (pos + size > file_get_size(desc)) {
		size = file_get_size(desc) - pos;
	}

	memcpy(buf, (char *) (uintptr_t) (fi->start_pos + pos), size);

	return size;
}

static int initfs_ioctl(struct file_desc *desc, int request, void *data) {
	char **p_addr;
	struct initfs_file_info *fi;

	fi = file_get_inode_data(desc);

	p_addr = data;

	*p_addr = (char*) (uintptr_t) fi->start_pos;

	return 0;
}

static size_t initfs_write(struct file_desc *desc, void *buf, size_t size) {
	return -EACCES;
}

struct file_operations initfs_fops = {
	.read  = initfs_read,
	.write = initfs_write,
	.ioctl = initfs_ioctl,
};
