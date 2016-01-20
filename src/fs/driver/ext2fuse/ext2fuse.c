/**
 * @file
 * @brief ext2fuse support
 *
 * @date 16.11.2015
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include <fs/dvfs.h>
#include <kernel/printk.h>
#include <embox/unit.h>
// Allocates embox-specific requests for the FUSE
#include <fuse_req_alloc.h>

// Needed by fuse_common.h
#define _FILE_OFFSET_BITS 64

// it is from fuse-ext2fs.c
#define FUSE_USE_VERSION 25
#include <fuse_lowlevel.h>
#include <fuse_opt.h>
#include <fuse_kernel.h>

#define FUSE_MAX_NAMELEN 255

#include <fs/fuse_module.h>
FUSE_MODULE_DEF("ext2fuse", "ext2fuse");

struct fuse_sb_priv_data ext2fuse_sb_priv_data;

static int ext2fuse_fill_sb(struct super_block *sb, struct file *bdev_file) {
	assert(sb);

	sb->sb_data = &ext2fuse_sb_priv_data;

	sb->sb_iops = (struct inode_operations *)&fuse_iops;
	sb->sb_fops = (struct file_operations *)&fuse_fops;
	sb->sb_ops  = (struct super_block_operations *)&fuse_sbops;
	sb->bdev = NULL;

	return 0;
}

static int ext2fuse_mount_end(struct super_block *sb) {
	return 0;
}

struct dumb_fs_driver ext2fuse_dumb_driver = {
	.name      = "ext2fuse",
	.fill_sb   = ext2fuse_fill_sb,
	.mount_end = ext2fuse_mount_end,
};

ARRAY_SPREAD_DECLARE(struct dumb_fs_driver *, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &ext2fuse_dumb_driver);
