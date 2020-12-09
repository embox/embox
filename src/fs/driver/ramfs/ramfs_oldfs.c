/**
 * @file
 * @brief Tmp file system
 *
 * @date 12.11.12
 * @author Andrey Gazukin
 */

#include <errno.h>

#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/super_block.h>

#include "ramfs.h"

static int ramfs_create(struct inode *parent_node, struct inode *node) {
	struct ramfs_file_info *fi;

	if (!node_is_directory(node)) {
		fi = ramfs_file_alloc(node);
		if (NULL == fi) {
			return -ENOMEM;
		}

		inode_priv_set(node, fi);
	}

	return 0;
}

struct inode_operations ramfs_iops;
struct super_block_operations { };
struct super_block_operations ramfs_sbops;
static int ramfs_mount(struct super_block *sb, struct inode *dest) {
	return 0;
}

static int ramfs_truncate(struct inode *node, off_t length) {
	assert(node);

	if (length > MAX_FILE_SIZE) {
		return -EFBIG;
	}

	inode_size_set(node, length);

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
	.name    = "ramfs",
	.fill_sb = ramfs_fill_sb,
	.file_op = &ramfs_fops,
	.fsop    = &ramfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(ramfs_driver);
