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
#include <fs/file_operation.h>
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

struct super_block_operations { };
struct super_block_operations fat_sbops;
extern int fat_fill_sb(struct super_block *sb, const char *source);
extern int fat_clean_sb(struct super_block *sb);

struct inode_operations fat_iops = {
	.iterate = fat_iterate,
};

static int fatfs_mount(struct super_block *sb, struct inode *dest) {
	/* Do nothing */
	return 0;
}

static int fatfs_create(struct inode *i_dir, struct inode *i_new) {
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	struct dirinfo *di;
	char *name;

	assert(i_dir && i_new);

	inode_size_set(i_new, 0);

	di = inode_priv(i_dir);

	name = i_new->name;

	fsi = di->fi.fsi;

	if (S_ISDIR(i_new->i_mode)) {
		struct dirinfo *new_di;
		new_di = fat_dirinfo_alloc();
		if (!new_di) {
			return -ENOMEM;
		}
		new_di->p_scratch = fat_sector_buff;
		fi = &new_di->fi;
	} else {
		fi = fat_file_alloc();
		if (!fi) {
			return -ENOMEM;
		}
	}

	inode_priv_set(i_new, fi);

	fi->volinfo = &fsi->vi;
	fi->fdi     = di;
	fi->fsi     = fsi;
	fi->mode   |= i_new->i_mode;

	if (0 != fat_create_file(fi, di, name, fi->mode)) {
		return -EIO;
	}

	return 0;

	return 0;
}
extern int fat_format(struct block_dev *dev, void *priv);
static struct fsop_desc fatfs_fsop = {
	.format = fat_format,
	.mount = fatfs_mount,
	.create_node = fatfs_create,
	.delete_node = fat_delete,
	.truncate = fat_truncate,
	.umount_entry = fatfs_umount_entry,
};

static const struct fs_driver fatfs_driver = {
	.name     = "vfat",
	.fill_sb  = fat_fill_sb,
	.clean_sb = fat_clean_sb,
	.file_op  = &fat_fops,
	.fsop     = &fatfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(fatfs_driver);
