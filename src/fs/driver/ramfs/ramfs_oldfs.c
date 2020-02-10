/**
 * @file
 * @brief Tmp file system
 *
 * @date 12.11.12
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include <util/array.h>
#include <util/indexator.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h> /* PAGE_SIZE() */
#include <mem/page.h>

#include <fs/file_desc.h>
#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/file_operation.h>
#include <fs/path.h>

#include <util/math.h>
#include <util/err.h>

#include <embox/unit.h>
#include <drivers/block_dev.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>
#include "ramfs.h"

/* ramfs filesystem description pool */
POOL_DEF(ramfs_fs_pool, struct ramfs_fs_info, RAMFS_DESCRIPTORS);

/* ramfs file description pool */
POOL_DEF(ramfs_file_pool, struct ramfs_file_info, RAMFS_FILES);

INDEX_DEF(ramfs_file_idx, 0, RAMFS_FILES);

static int ramfs_format(struct block_dev *bdev, void *priv);
static int ramfs_mount(const char *source, struct inode *dest);

static struct ramfs_file_info *ramfs_create_file(struct nas *nas) {
	struct ramfs_file_info *fi;
	size_t fi_index;

	fi = pool_alloc(&ramfs_file_pool);
	if (!fi) {
		return NULL;
	}

	fi_index = index_alloc(&ramfs_file_idx, INDEX_MIN);
	if (fi_index == INDEX_NONE) {
		pool_free(&ramfs_file_pool, fi);
		return NULL;
	}

	fi->index = fi_index;
	fi->fsi = nas->fs->sb_data;
	nas->fi->ni.size = 0;

	return fi;
}

static int ramfs_create(struct inode *parent_node, struct inode *node) {
	struct nas *nas;

	nas = node->nas;
	nas->fs = parent_node->nas->fs;

	if (!node_is_directory(node)) {
		if (!(nas->fi->privdata = ramfs_create_file(nas))) {
			return -ENOMEM;
		}
	}

	return 0;
}

static int ramfs_delete(struct inode *node) {
	struct ramfs_file_info *fi;
	struct nas *nas;

	nas = node->nas;
	fi = nas->fi->privdata;

	if (!node_is_directory(node)) {
		index_free(&ramfs_file_idx, fi->index);
		pool_free(&ramfs_file_pool, fi);
	}

	return 0;
}

static int ramfs_truncate(struct inode *node, off_t length) {
	struct nas *nas = node->nas;

	if (length > MAX_FILE_SIZE) {
		return -EFBIG;
	}

	nas->fi->ni.size = length;

	return 0;
}

static int ramfs_format(struct block_dev *bdev, void *priv) {
	if (NULL == bdev) {
		return -ENODEV;
	}

	if (MAX_FILE_SIZE > bdev->size) {
		return -ENOSPC;
	}

	return 0;
}

static int ramfs_mount(const char *source, struct inode *dest) {
	struct block_dev *bdev;
	struct nas *dir_nas;
	struct ramfs_file_info *fi;
	struct ramfs_fs_info *fsi;

	dir_nas = dest->nas;

	bdev = bdev_by_path(source);
	if (NULL == bdev) {
		return -ENODEV;
	}

	dir_nas->fs = super_block_alloc("ramfs", bdev);
	if (NULL == dir_nas->fs) {
		return -ENOMEM;
	}

	/* allocate this fs info */
	if (NULL == (fsi = pool_alloc(&ramfs_fs_pool))) {
		super_block_free(dir_nas->fs);
		return -ENOMEM;
	}
	memset(fsi, 0, sizeof(struct ramfs_fs_info));
	dir_nas->fs->sb_data = fsi;

	fsi->block_per_file = MAX_FILE_SIZE / PAGE_SIZE();
	fsi->block_size = PAGE_SIZE();
	fsi->numblocks = bdev->size / PAGE_SIZE();
	fsi->bdev = bdev;

	/* allocate this directory info */
	if(NULL == (fi = pool_alloc(&ramfs_file_pool))) {
		return -ENOMEM;
	}
	memset(fi, 0, sizeof(struct ramfs_file_info));
	fi->index = fi->mode = 0;
	dir_nas->fi->privdata = (void *) fi;

	return 0;
}

static struct fsop_desc ramfs_fsop = {
	.format = ramfs_format,
	.mount = ramfs_mount,
	.create_node = ramfs_create,
	.delete_node = ramfs_delete,

	.truncate = ramfs_truncate,
};

static struct fs_driver ramfs_driver = {
	.name = "ramfs",
	.file_op = &ramfs_fops,
	.fsop = &ramfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(ramfs_driver);
