/**
 * @file
 * @brief
 *
 * @date 31.07.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include <mem/phymem.h>

#include <embox/unit.h>

#include <fs/vfs.h>
#include <fs/node.h>
#include <fs/file_desc.h>
#include <fs/bcache.h>
#include <fs/file_operation.h>

#include <drivers/block_dev.h>

EMBOX_UNIT_INIT(blockdev_init);

static struct idesc *bdev_open(struct node *node, struct file_desc *file_desc, int flags) {
	return &file_desc->idesc;
}

static int bdev_close(struct file_desc *desc) {
	return 0;
}

static size_t bdev_read(struct file_desc *desc, void *buf, size_t size) {
	int n_read = block_dev_read_buffered((struct block_dev *) desc->node->nas->fi->privdata,
			buf, size, desc->cursor);
	if (n_read > 0) {
		desc->cursor += n_read;
	}
	return n_read;
}

static size_t bdev_write(struct file_desc *desc, void *buf, size_t size) {
	int n_write = block_dev_write_buffered((struct block_dev *) desc->node->nas->fi->privdata,
			buf, size, desc->cursor);
	if (n_write > 0) {
		desc->cursor += n_write;
	}

	return n_write;
}

static struct file_operations blockdev_fop = {
	.open = bdev_open,
	.close = bdev_close,
	.read = bdev_read,
	.write = bdev_write,
};

static struct filesystem *blockdev_fs;

static int blockdev_init(void) {
	blockdev_fs = filesystem_create("empty");
	blockdev_fs->file_op = &blockdev_fop;

	return 0;
}

struct block_dev *block_dev_create(const char *path, void *driver, void *privdata) {
	struct block_dev *bdev;
	struct path node, root;
	struct nas *nas;
	struct node_fi *node_fi;
	char full_path[PATH_MAX];

	/* XXX  Workaround for compatibility with DVFS.
	 * Drivers which are used in both vfs and dvfs
	 * currently use different namings:
	 *  block_dev_create(blk0, ...) for dvfs
	 *  block_dev_create(/dev/blk0, ...) for vfs.
	 * So in case if block_dev_create was called without /dev
	 * just prepend the path with /dev. */
	if (strncmp("/dev", path, 4)) {
		strcpy(full_path, "/dev/");
		strncat(full_path, path, PATH_MAX - strlen("/dev") - 1);
	} else {
		strncpy(full_path, path, PATH_MAX);
	}

	bdev = block_dev_create_common(full_path, driver, privdata);
	if (NULL == bdev) {
		return NULL;
	}

	vfs_get_root_path(&root);

	if (0 != vfs_create(&root, full_path, S_IFBLK | S_IRALL | S_IWALL, &node)) {
		block_dev_free(bdev);
		return NULL;
	}

	bdev->dev_vfs_info = node.node;
	nas = node.node->nas;
	nas->fs = blockdev_fs;
	node_fi = nas->fi;
	node_fi->privdata = bdev;
	node_fi->ni.size = 0;/*TODO*/
	node_fi->ni.mtime = 0;/*TODO*/

	return bdev;
}

int block_dev_destroy(void *dev) {
	struct block_dev *bdev;

	bdev = block_dev(dev);
	vfs_del_leaf(bdev->dev_vfs_info);
	block_dev_free(bdev);

	return 0;
}

