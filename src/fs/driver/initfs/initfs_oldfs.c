/**
 * @file
 * @details Read-only filesystem with direct address space mapping.
 *
 * @date 29.06.09
 * @author Anton Bondarev
 *	        - initial implementation
 * @author Nikolay Korotky
 *	        - rework using vfs
 * @author Eldar Abusalimov
 *	        - rework mount to use cpio_parse_entry
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <cpio.h>
#include <stdarg.h>
#include <limits.h>

#include <fs/dir_context.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/vfs.h>
#include <kernel/printk.h>
#include <framework/mod/options.h>

#include "initfs.h"

static struct inode_operations initfs_iops = {
	.iterate = initfs_iterate,
};

static int initfs_mount(struct super_block *sb, struct inode *dest) {
	extern char _initfs_start, _initfs_end;
	struct initfs_file_info *fi;

	if (&_initfs_start == &_initfs_end) {
		return -1;
	}

	fi = initfs_file_alloc();
	if (fi == NULL) {
		return -ENOMEM;
	}

	memset(fi, 0, sizeof(*fi));
	inode_priv_set(dest, fi);

	dest->i_ops = &initfs_iops;

	return 0;
}

static int initfs_create_node(struct inode *parent_node, struct inode *node) {
	return -EACCES;
}

extern struct file_operations initfs_fops;

static struct fsop_desc initfs_fsop = {
	.mount = initfs_mount,
	.create_node = initfs_create_node,
};

static struct fs_driver initfs_driver = {
	.name = "initfs",
	.file_op = &initfs_fops,
	.fsop = &initfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(initfs_driver);
