/**
 * @file
 * @brief Implementation of FAT driver
 *
 * @date 28.03.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <libgen.h>

#include <util/err.h>

#include <fs/file_desc.h>
#include <fs/super_block.h>
#include <fs/fs_driver.h>
#include <fs/inode_operation.h>
#include <fs/inode.h>
#include <fs/vfs.h>
#include <drivers/block_dev.h>

#include "fat.h"

static int fatfs_umount_entry(struct inode *node) {
	if (node_is_directory(node)) {
		fat_dirinfo_free(inode_priv(node));
	} else {
		fat_file_free(inode_priv(node));
	}

	return 0;
}

extern struct file_operations fat_fops;
extern int fat_destroy_inode(struct inode *inode);
struct super_block_operations fat_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = fat_destroy_inode,
};

extern int fat_fill_sb(struct super_block *sb, const char *source);
extern int fat_clean_sb(struct super_block *sb);

struct inode_operations fat_iops = {
	.iterate = fat_iterate,
	.truncate = fat_truncate,
};

static int fatfs_mount(struct super_block *sb, struct inode *dest) {
	/* Do nothing */
	return 0;
}

extern int fat_create(struct inode *i_new, struct inode *i_dir, int mode);
extern int fat_format(struct block_dev *dev, void *priv);
static struct fsop_desc fatfs_fsop = {
	.mount = fatfs_mount,
	.create_node = fat_create,
	.delete_node = fat_delete,
	.truncate = fat_truncate,
	.umount_entry = fatfs_umount_entry,
};

static const struct fs_driver fatfs_driver = {
	.name     = "vfat",
	.format = fat_format,
	.fill_sb  = fat_fill_sb,
	.clean_sb = fat_clean_sb,
//	.file_op  = &fat_fops,
	.fsop     = &fatfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(fatfs_driver);
