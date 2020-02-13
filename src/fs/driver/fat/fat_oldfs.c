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

static int fat_create_dir_entry(struct nas *parent_nas,
		struct dirinfo *parent_di,
		struct fat_dirent *self_de) {
	struct fat_dirent de;
	char name[PATH_MAX];
	struct nas *nas;
	struct fat_file_info *fi;
	struct inode *node;
	mode_t mode;
	int ret;
	struct dirinfo *new_di = NULL;
	struct fat_fs_info *fsi = parent_nas->fs->sb_data;

	fat_reset_dir(parent_di);
	read_dir_buf(parent_di);

	while (DFS_EOF != fat_get_next_long(parent_di, &de, name)) {
		if (de.name[0] == 0) {
			continue;
		}

		if ((0 == strcmp(name, ".")) || (0 == strcmp(name, ".."))) {
			continue;
		}

		if (de.attr & ATTR_VOLUME_ID) {
			continue;
		}

		if (de.attr & ATTR_DIRECTORY) {
			mode = S_IFDIR;

			if (NULL == (new_di = fat_dirinfo_alloc())) {
				return -ENOMEM;
			}

			fi = &new_di->fi;
			new_di->p_scratch = fat_sector_buff;
		} else {
			mode = S_IFREG;

			if (NULL == (fi = fat_file_alloc())) {
				return -ENOMEM;
			}
		}

		if (NULL == (node = vfs_subtree_create_child(parent_nas->node, name, mode))) {
			if (de.attr & ATTR_DIRECTORY) {
				fat_dirinfo_free(new_di);
			} else {
				fat_file_free(fi);
			}
			return -ENOMEM;
		}

		memset(fi, 0, sizeof(*fi));

		fi->fsi          = parent_nas->fs->sb_data;
		fi->filelen      = fat_direntry_get_size(&de);
		fi->dirsector    = fat_current_dirsector(parent_di);
		fi->diroffset    = parent_di->currententry - 1;
		fi->cluster      = fat_direntry_get_clus(&de);
		fi->firstcluster = fi->cluster;
		fi->filelen      = fat_direntry_get_size(&de);
		fi->volinfo      = &fsi->vi;
		fi->fdi          = parent_di;
		fi->mode         = mode;

		inode_priv_set(node, fi);
		nas = node->nas;
		nas->fi->ni.size = fi->filelen;

		if (de.attr & ATTR_DIRECTORY) {
			if ((ret = fat_create_dir_entry(nas, (void *) fi, &de))) {
					return ret;
			}
			/* Children directories use the same read buffer,
			 * so we need to reread relevant content */
			read_dir_buf(parent_di);
		}
	}

	return ENOERR;
}

static int fatfs_umount_entry(struct inode *node) {
	if (node_is_directory(node)) {
		fat_dirinfo_free(inode_priv(node));
	} else {
		fat_file_free(inode_priv(node));
	}

	return 0;
}

extern struct file_operations fat_fops;

struct inode_operations fat_iops;
struct super_block_operations { };
struct super_block_operations fat_sbops;
extern int fat_fill_sb(struct super_block *sb, const char *source);
extern int fat_clean_sb(struct super_block *sb);

static int fatfs_mount(struct super_block *sb, struct inode *dest) {
	struct dirinfo *di;
	struct fat_dirent de;

	di = inode_priv(dest);

	return fat_create_dir_entry(dest->nas, di, &de);
}

static int fatfs_create(struct inode *parent_node, struct inode *node) {
	struct nas *nas;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	struct dirinfo *di;

	assert(parent_node && node);

	nas = node->nas;
	nas->fi->ni.size = 0;

	di = inode_priv(parent_node);

	if (S_ISDIR(node->i_mode)) {
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

	inode_priv_set(node, fi);

	fsi = di->fi.fsi;
	*fi = (struct fat_file_info) {
		.fsi     = fsi,
		.volinfo = &fsi->vi,
		.fdi     = di,
		.mode    = node->i_mode,
	};

	if (0 != fat_create_file(fi, di, node->name, node->i_mode)) {
		return -EIO;
	}

	return 0;
}

static int fatfs_delete(struct inode *node) {
	struct fat_file_info *fi;

	fi = inode_priv(node);

	if (fat_unlike_file(fi, (uint8_t *) fat_sector_buff)) {
		return -1;
	}

	if (S_ISDIR(node->i_mode)) {
		fat_dirinfo_free((void *) fi);
	} else {
		fat_file_free(fi);
	}

	return 0;
}

static int fatfs_truncate(struct inode *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	/* TODO realloc blocks*/

	return 0;
}

extern int fat_format(struct block_dev *dev, void *priv);
static struct fsop_desc fatfs_fsop = {
	.format = fat_format,
	.mount = fatfs_mount,
	.create_node = fatfs_create,
	.delete_node = fatfs_delete,
	.truncate = fatfs_truncate,
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
