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

#include <mem/misc/pool.h>
#include <mem/phymem.h>
#include <util/array.h>
#include <util/indexator.h>
#include <util/math.h>

#include <embox/unit.h>

#include <fs/vfs.h>
#include <fs/node.h>
#include <fs/file_desc.h>
#include <fs/bcache.h>
#include <fs/file_operation.h>

#include <embox/block_dev.h>

#define MAX_DEV_QUANTITY 16 /* XXX */

EMBOX_UNIT_INIT(blockdev_init);

POOL_DEF(blockdev_pool, struct block_dev, MAX_DEV_QUANTITY);
INDEX_DEF(block_dev_idx,0,MAX_DEV_QUANTITY);

static struct block_dev *devtab[MAX_DEV_QUANTITY];


static int bdev_open(struct node *node, struct file_desc *file_desc, int flags) {
	return 0;
}

static int bdev_close(struct file_desc *desc) {
	return 0;
}

static size_t bdev_read(struct file_desc *desc, void *buf, size_t size) {
	int n_read = block_dev_read_buffered((block_dev_t *) desc->node->nas->fi->privdata,
			buf, size, desc->cursor);
	if (n_read > 0) {
		desc->cursor += n_read;
	}
	return n_read;
}

static size_t bdev_write(struct file_desc *desc, void *buf, size_t size) {
	int n_write = block_dev_write_buffered((block_dev_t *) desc->node->nas->fi->privdata,
			buf, size, desc->cursor);
	if (n_write > 0) {
		desc->cursor += n_write;
	}

	return n_write;
}

static int bdev_ioctl(struct file_desc *desc, int request, ...) {
	return -ENOTSUP;
}

static struct kfile_operations blockdev_fop = {
	.open = bdev_open,
	.close = bdev_close,
	.read = bdev_read,
	.write = bdev_write,
	.ioctl = bdev_ioctl,

};

static struct filesystem *blockdev_fs;

static int blockdev_init(void) {
	blockdev_fs = filesystem_create("empty");
	blockdev_fs->file_op = &blockdev_fop;

	return 0;
}

struct block_dev *block_dev_create(char *path, void *driver, void *privdata) {
	block_dev_t *bdev;
	size_t bdev_id;
	struct path node, root;
	struct nas *nas;
	struct node_fi *node_fi;

	bdev = (block_dev_t *) pool_alloc(&blockdev_pool);
	if (NULL == bdev) {
		return NULL;
	}

	memset(bdev, 0, sizeof(block_dev_t));

	bdev_id = index_alloc(&block_dev_idx, INDEX_MIN);
	if (bdev_id == INDEX_NONE) {
		pool_free(&blockdev_pool, bdev);
		return NULL;
	}
	bdev->id = (dev_t)bdev_id;

	devtab[bdev->id] = bdev;

	bdev->driver = driver;
	bdev->privdata = privdata;

	vfs_get_root_path(&root);

	if (0 != vfs_create(&root, path, S_IFBLK | S_IRALL | S_IWALL, &node)) {
		index_free(&block_dev_idx, bdev->id);
		pool_free(&blockdev_pool, bdev);
		return NULL;
	}

	strncpy (bdev->name, node.node->name, NAME_MAX);
	bdev->dev_vfs_info = node.node;

	nas = node.node->nas;
	nas->fs = blockdev_fs;
	node_fi = nas->fi;
	node_fi->privdata = bdev;
	node_fi->ni.size = 0;/*TODO*/
	node_fi->ni.mtime = 0;/*TODO*/

	return bdev;
}

int block_dev_destroy (void *dev) {
	block_dev_t *bdev;

	bdev = block_dev(dev);

	vfs_del_leaf(bdev->dev_vfs_info);

	block_dev_cache_free(bdev);

	devtab[bdev->id] = NULL;
	index_free(&block_dev_idx, bdev->id);

	pool_free(&blockdev_pool, bdev);

	return 0;
}

