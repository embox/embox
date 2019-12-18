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
#include <fs/file_system.h>
#include <fs/fs_driver.h>
#include <fs/node.h>
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
	struct fat_fs_info *fsi = parent_nas->fs->fsi;

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
			struct dirinfo *new_di;
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
			fat_file_free(fi);
			return -ENOMEM;
		}

		memset(fi, 0, sizeof(*fi));

		fi->fsi = parent_nas->fs->fsi;
		fi->filelen      = fat_direntry_get_size(&de);
		fi->dirsector    = fat_current_dirsector(parent_di);
		fi->diroffset    = parent_di->currententry - 1;
		fi->cluster      = fat_direntry_get_clus(&de);
		fi->firstcluster = fi->cluster;
		fi->filelen      = fat_direntry_get_size(&de);
		fi->volinfo      = &fsi->vi;
		fi->fdi          = parent_di;
		fi->mode         = mode;

		nas = node->nas;
		nas->fs = parent_nas->fs;
		nas->fi->privdata = fi;
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

static int fat_mount_files(struct nas *dir_nas) {
	uint32_t pstart, psize;
	uint8_t pactive, ptype;
	struct dirinfo *di;
	struct fat_dirent de;
	struct fat_fs_info *fsi;

	assert(dir_nas);

	di = dir_nas->fi->privdata;
	fsi = dir_nas->fs->fsi;

	pstart = fat_get_ptn_start(dir_nas->fs->bdev, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) {
		return -1;
	}
	if (fat_get_volinfo(dir_nas->fs->bdev, &fsi->vi, pstart)) {
		return -1;
	}

	if (fat_open_rootdir(fsi, di)) {
		return -EBUSY;
	}

	return fat_create_dir_entry(dir_nas, di, &de);
}

static void fat_free_fs(struct nas *nas) {
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;

	if (NULL != nas->fs) {
		fsi = nas->fs->fsi;

		if(NULL != fsi) {
			fat_fs_free(fsi);
		}
		filesystem_free(nas->fs);
	}

	if (NULL != (fi = nas->fi->privdata)) {
		fat_dirinfo_free(nas->fi->privdata);
	}
}

static int fat_umount_entry(struct nas *nas) {
	struct inode *child;

	if (node_is_directory(nas->node)) {
		while (NULL != (child = vfs_subtree_get_child_next(nas->node, NULL))) {
			if (node_is_directory(child)) {
				fat_umount_entry(child->nas);
				fat_dirinfo_free(child->nas->fi->privdata);
			} else {
				fat_file_free(child->nas->fi->privdata);
			}
			vfs_del_leaf(child);
		}
	}

	return 0;
}

extern struct file_operations fat_fops;

static int fatfs_mount(void *dev, void *dir) {
	struct inode *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct dirinfo *di;
	struct fat_fs_info *fsi;
	struct node_fi *dev_fi;
	int rc;

	dev_node = dev;
	dev_nas = dev_node->nas;
	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
		rc = -ENODEV;
		goto error;
	}

	if (NULL == (dir_nas->fs = filesystem_create("vfat"))) {
		rc = -ENOMEM;
		goto error;
	}

	dir_nas->fs->bdev = dev_fi->privdata;

	if (NULL == (fsi = fat_fs_alloc())) {
		rc = -ENOMEM;
		goto error;
	}
	memset(fsi, 0, sizeof(struct fat_fs_info));
	dir_nas->fs->fsi = fsi;

	/* allocate this directory info */
	if (NULL == (di = fat_dirinfo_alloc())) {
		rc = -ENOMEM;
		goto error;
	}
	memset(di, 0, sizeof(struct dirinfo));
	dir_nas->fi->privdata = (void *) di;
	di->fi.fsi = fsi;
	di->p_scratch = fat_sector_buff;

	fsi->bdev = dir_nas->fs->bdev;
	fsi->root = dir_node;

	return fat_mount_files(dir_nas);

error:
	fat_free_fs(dir_nas);

	return rc;
}

static int fatfs_create(struct inode *parent_node, struct inode *node) {
	struct nas *nas;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	struct dirinfo *di;

	assert(parent_node && node);

	nas = node->nas;
	nas->fi->ni.size = 0;

	di = (void *) parent_node->nas->fi->privdata;

	if (S_ISDIR(node->mode)) {
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

	fsi = di->fi.fsi;
	nas->fi->privdata = fi;
	nas->fs = parent_node->nas->fs;
	*fi = (struct fat_file_info) {
		.fsi     = fsi,
		.volinfo = &fsi->vi,
		.fdi     = di,
		.mode    = node->mode,
	};

	if (0 != fat_create_file(fi, di, node->name, node->mode)) {
		return -EIO;
	}

	return 0;
}

static int fatfs_delete(struct inode *node) {
	struct nas *nas;
	struct fat_file_info *fi;

	nas = node->nas;
	fi = nas->fi->privdata;

	if (fat_unlike_file(fi, (uint8_t *) fat_sector_buff)) {
		return -1;
	}

	if (S_ISDIR(node->mode)) {
		fat_dirinfo_free((void *) fi);
	} else {
		fat_file_free(fi);
	}

	vfs_del_leaf(node);
	return 0;
}

static int fatfs_truncate(struct inode *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	/* TODO realloc blocks*/

	return 0;
}

static int fatfs_umount(void *dir) {
	struct inode *dir_node;
	struct nas *dir_nas;

	dir_node = dir;
	dir_nas = dir_node->nas;

	fat_umount_entry(dir_nas);

	fat_free_fs(dir_nas);

	return 0;
}

extern int fat_format(struct block_dev *dev, void *priv);
static struct fsop_desc fatfs_fsop = {
	.format = fat_format,
	.mount = fatfs_mount,
	.create_node = fatfs_create,
	.delete_node = fatfs_delete,
	.truncate = fatfs_truncate,
	.umount = fatfs_umount,
};

static const struct fs_driver fatfs_driver = {
	.name = "vfat",
	.file_op = &fat_fops,
	.fsop = &fatfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(fatfs_driver);
