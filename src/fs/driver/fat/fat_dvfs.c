/**
 * @file  fat_dvfs.c
 * @brief Implementation of FAT driver for DVFS
 *
 * @date   11 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>

#include <fs/fat.h>
#include <fs/dvfs.h>
#include <framework/mod/options.h>

#define FAT_MAX_SECTOR_SIZE OPTION_GET(NUMBER, fat_max_sector_size)
uint8_t fat_sector_buff[FAT_MAX_SECTOR_SIZE];

extern struct file_operations fat_fops;

/* @brief Figure out if node at specific path exists or not
 * @note  Assume dir is root
 * @param name Full path to the extected node
 * @param dir  Not used now
 *
 * @return Pointer of inode or NULL if not found
 */
static struct inode *fat_ilookup(char const *name, struct dentry const *dir) {
	struct fat_file_info *fi;
	int res;
	struct inode *node;

	assert(name);

	if (NULL == (node = dvfs_alloc_inode(dir->d_sb)))
		return NULL;
	if (NULL == (fi   = fat_file_alloc())) {
		dvfs_destroy_inode(node);
		return NULL;
	}
	node->i_data = fi;
	res = fat_open_file(fi, (uint8_t*) name, 0, fat_sector_buff, &node->length);

	if (res == DFS_OK) {
		return node;
	} else {
		dvfs_destroy_inode(node);
		return NULL;
	}
}

/* @brief Create new file or directory
 * @param d_new Dentry related to new file
 * @param d_dir Dentry realted to the parent
 * @param mode  Used to figure out file type
 *
 * @return Pointer to inode of the new file
 */
static struct inode *fat_create(struct dentry *d_new,
                                struct dentry *d_dir, int mode) {
	return NULL;
}

static int fat_open(struct inode *node, struct file *desc) {
	desc->f_inode = node;
	desc->f_ops   = &fat_fops;
	desc->pos     = 0;

	return 0;
}

static int fat_close(struct file *desc) {
	return 0;
}

static size_t fat_read(struct file *desc, void *buf, size_t size) {
	uint32_t res;
	return fat_read_file(desc->f_inode->i_data,
	                     fat_sector_buff, buf, &res, size);
}

static size_t fat_write(struct file *desc, void *buf, size_t size) {
	uint32_t res;
	return fat_write_file(desc->f_inode->i_data, fat_sector_buff,
	                      buf, &res, size, &desc->f_inode->length);
}

static struct super_block *fat_alloc_sb(struct block_dev *bdev) {
	return NULL;
}

/* Declaration of operations */
struct inode_operations fat_iops = {
	.create   = fat_create,
	.lookup   = fat_ilookup,
	//.mkdir    = NULL,
	//.rmdir    = NULL,
	//.iterate  = fat_iterate,
	//.truncate = fat_itruncate,
	//.pathname = fat_pathname,
};

struct file_operations fat_fops = {
	.open = fat_open,
	.close = fat_close,
	.write = fat_write,
	.read = fat_read,
	//.ioctl = NULL,
};

static struct dumb_fs_driver dfs_fat_driver = {
	.name = "vfat",
	.alloc_sb = &fat_alloc_sb,
};

ARRAY_SPREAD_DECLARE(struct dumb_fs_driver *, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &dfs_fat_driver);

