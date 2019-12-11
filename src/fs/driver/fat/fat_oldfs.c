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

/* VFS-independent functions */
static struct fat_file_info *fat_fi_alloc(struct nas *nas, void *fs) {
	struct fat_file_info *fi;

	fi = fat_file_alloc();
	if (fi) {
		memset(fi, 0, sizeof(*fi));
		nas->fi->privdata = fi;
		nas->fs = fs;
	}

	return fi;
}

static int fat_create_dir_entry(struct nas *parent_nas) {
	struct dirinfo di;
	struct fat_dirent de;
	char name[MSDOS_NAME + 2];
	char dir_path[PATH_MAX];
	char dir_buff[FAT_MAX_SECTOR_SIZE];
	struct nas *nas;
	struct fat_file_info *fi;
	struct inode *node;
	mode_t mode;

	di.p_scratch = (uint8_t *) dir_buff;

	vfs_get_relative_path(parent_nas->node, dir_path, PATH_MAX);

	if (fat_open_dir(parent_nas->fs->fsi, (uint8_t *) dir_path, &di)) {
		return -ENODEV;
	}

	while (DFS_EOF != fat_get_next(&di, &de)) {
		if (de.name[0] == 0)
			continue;

		path_dir_to_canonical(name, (char *) de.name, de.attr & ATTR_DIRECTORY);
		if ((0 == strncmp((char *) de.name, ".  ", 3)) ||
			(0 == strncmp((char *) de.name, ".. ", 3))) {
			continue;
		}

		if (NULL == (fi = fat_file_alloc())) {
			return -ENOMEM;
		}

		mode = (de.attr & ATTR_DIRECTORY) ? S_IFDIR : S_IFREG;

		if (NULL == (node = vfs_subtree_create_child(parent_nas->node, name, mode))) {
			fat_file_free(fi);
			return -ENOMEM;
		}

		memset(fi, 0, sizeof(*fi));

		nas = node->nas;
		nas->fs = parent_nas->fs;
		nas->fi->privdata = fi;
		fi->fsi = parent_nas->fs->fsi;

		if (de.attr & ATTR_DIRECTORY) {
			fat_create_dir_entry(nas);
		}
	}

	return ENOERR;
}

static int fat_mount_files(struct nas *dir_nas) {
	uint32_t cluster;
	struct inode *node;
	struct nas *nas;
	uint32_t pstart, psize;
	uint8_t pactive, ptype;
	struct dirinfo di;
	struct fat_dirent de;
	uint8_t name[PATH_MAX];
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	mode_t mode;

	assert(dir_nas);

	memset(&di, 0, sizeof(struct dirinfo));

	fsi = dir_nas->fs->fsi;

	pstart = fat_get_ptn_start(dir_nas->fs->bdev, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) {
		return -1;
	}
	if (fat_get_volinfo(dir_nas->fs->bdev, &fsi->vi, pstart)) {
		return -1;
	}
	di.p_scratch = fat_sector_buff;
	di.fi.fsi = fsi;
	if (fat_open_dir(fsi, (uint8_t *) ROOT_DIR, &di)) {
		return -EBUSY;
	}
	/* move out from first root directory entry table*/
	if (di.currententry == 0) {
		/* Need to get directory data from drive */
		fat_reset_dir(&di);
	}

	while (DFS_OK == (cluster = fat_get_next_long(&di, &de, (char *) name)) || cluster == DFS_ALLOCNEW) {
		mode = (de.attr & ATTR_DIRECTORY) ? S_IFDIR : S_IFREG;

		if (NULL == (fi = fat_file_alloc())) {
			return -ENOMEM;
		}
		if (NULL == (node = vfs_subtree_create_child(dir_nas->node, (const char *) name, mode))) {
			fat_file_free(fi);
			return -ENOMEM;
		}

		memset(fi, 0, sizeof(struct fat_file_info));
		fi->fsi = fsi;
		fi->filelen = fat_direntry_get_size(&de);

		nas = node->nas;
		nas->fs = dir_nas->fs;
		nas->fi->privdata = (void *)fi;
		nas->fi->ni.size = fi->filelen;

		if (de.attr & ATTR_DIRECTORY) {
			fat_create_dir_entry(nas);
		}
	}
	return DFS_OK;
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
		fat_file_free(fi);
	}
}

static int fat_umount_entry(struct nas *nas) {
	struct inode *child;

	if (node_is_directory(nas->node)) {
		while (NULL != (child = vfs_subtree_get_child_next(nas->node, NULL))) {
			if (node_is_directory(child)) {
				fat_umount_entry(child->nas);
			}

			fat_file_free(child->nas->fi->privdata);
			vfs_del_leaf(child);
		}
	}

	return 0;
}

/* File operations */
extern size_t fat_read(struct file_desc *desc, void *buf, size_t size);
extern size_t fat_write(struct file_desc *desc, void *buf, size_t size);
static struct file_operations fatfs_fop = {
	.read = fat_read,
	.write = fat_write,
};

static int fatfs_mount(void *dev, void *dir) {
	struct inode *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct fat_file_info *fi;
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
	if (NULL == (fi = fat_file_alloc())) {
		rc = -ENOMEM;
		goto error;
	}
	memset(fi, 0, sizeof(struct fat_file_info));
	dir_nas->fi->privdata = (void *) fi;
	((struct fat_fs_info *) dir_nas->fs->fsi)->bdev = dir_nas->fs->bdev;
	((struct fat_fs_info *) dir_nas->fs->fsi)->root = dir_node;

	return fat_mount_files(dir_nas);

error:
	fat_free_fs(dir_nas);

	return rc;
}

static int fatfs_create(struct inode *parent_node, struct inode *node) {
	struct nas *parent_nas, *nas;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	struct dirinfo di;
	char dir_buff[FAT_MAX_SECTOR_SIZE];
	char dir_path[PATH_MAX];
	char *dpath;
	char tmppath[PATH_MAX];

	assert(parent_node && node);

	nas = node->nas;
	parent_nas = parent_node->nas;
	nas->fi->ni.size = 0;

	memset(&di,0, sizeof(di));
	di.p_scratch = (uint8_t *) dir_buff;

	vfs_get_relative_path(parent_nas->node, dir_path, PATH_MAX);
	dpath = basename(dir_path);
	if (fat_open_dir(parent_nas->fs->fsi, (uint8_t *) dpath, &di)) {
		return -ENODEV;
	}

	if (NULL == fat_fi_alloc(nas, parent_nas->fs)) {
		return -ENOMEM;
	}

	vfs_get_relative_path(node, tmppath, PATH_MAX);

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	*fi = (struct fat_file_info) {
		.fsi = fsi,
		.volinfo = &fsi->vi,
	};

	if (0 != fat_create_file(fi, &di, tmppath, node->mode)) {
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
	fat_file_free(fi);

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
	.file_op = &fatfs_fop,
	.fsop = &fatfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(fatfs_driver);
