/**
 * @file  fat_dvfs.c
 * @brief Implementation of FAT driver for DVFS
 *
 * @date   11 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>

#include <fs/fat.h>
#include <fs/dvfs.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <util/math.h>

POOL_DEF(fat_dirinfo_pool, struct dirinfo, 4);

#define FAT_MAX_SECTOR_SIZE OPTION_GET(NUMBER, fat_max_sector_size)
uint8_t fat_sector_buff[FAT_MAX_SECTOR_SIZE];

extern struct file_operations fat_fops;

/* @brief Fill struct fat_file_info according to inode data
 * @param inode Source of data about file
 * @param fi    Structure to be filled
 */
static inline void fat_fi_from_inode(struct inode *inode, struct fat_file_info *fi) {

}

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
	*fi = (struct fat_file_info) {
		.fsi = node->i_sb->sb_data,
		.volinfo = &((struct fat_fs_info*)node->i_sb->sb_data)->vi,
	};
	res = fat_open_file(fi, (uint8_t*) name, 0, fat_sector_buff, &node->length);

	if (res == DFS_OK) {
		return node;
	} else {
		dvfs_destroy_inode(node);
		return NULL;
	}
}

/* @brief Create new file or directory
 * @param i_new Inode to be filled
 * @param i_dir Inode realted to the parent
 * @param mode  Used to figure out file type
 *
 * @return Negative error code
 */
static int fat_create(struct inode *i_new,
                                struct inode *i_dir, int mode) {
	int res;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	assert(i_new);
	assert(i_dir);
	/* TODO check file exists */

	fsi = i_dir->i_sb->sb_data;
	fi = fat_file_alloc();
	*fi = (struct fat_file_info) {
		.fsi     = fsi,
		.volinfo = &fsi->vi,
	};

	res = fat_create_file(fi, i_new->i_dentry->name, mode);

	return res;
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
	struct fat_file_info *fi = desc->f_inode->i_data;
	fat_read_file(fi, fat_sector_buff, buf, &res, min(size, fi->filelen - desc->pos));
	return res;
}

static size_t fat_write(struct file *desc, void *buf, size_t size) {
	uint32_t res;
	struct fat_file_info *fi = desc->f_inode->i_data;
	fi->mode |= O_RDWR; /* XXX */
	fat_write_file(desc->f_inode->i_data, fat_sector_buff,
	               buf, &res, size, &desc->f_inode->length);
	return res;
}

/* @brief Get next inode in directory
 * @param inode   Structure to be filled
 * @param parent  Inode of parent directory
 * @param dir_ctx Directory context
 *
 * @return Error code
 */
static int fat_iterate(struct inode *next, struct inode *parent, struct dir_ctx *ctx) {
	struct fat_fs_info *fsi;
	struct dirinfo *dirinfo;
	struct dirent de;
	char path[PATH_MAX];
	int res;

	if (!parent)
		strcpy(path, ROOT_DIR);

	assert(parent->i_sb);
	fsi = parent->i_sb->sb_data;

	if (ctx->fs_ctx == NULL) {
		dirinfo = pool_alloc(&fat_dirinfo_pool);
		*dirinfo = (struct dirinfo) {
			.p_scratch = fat_sector_buff,
		};
		dvfs_pathname(parent, path);
		fat_open_dir(fsi, (uint8_t*) path, dirinfo);
		ctx->fs_ctx = dirinfo;
	}

	dirinfo = ctx->fs_ctx;
	if (ctx->pos == 0)
		dirinfo->currententry = 0;
	//dirinfo->currententry = ctx->pos;
	while (DFS_EOF != (res = fat_get_next(fsi, dirinfo, &de))) {
		if (de.name[0] == 0)
			continue;
		else
			break;
	}

	if (DFS_OK != res)
		return -1;
	else
		strcpy(next->i_dentry->name, (char*) de.name);

	//pool_free(&fat_dirinfo_pool, dirinfo);
	//ctx->fs_ctx = 0;
	return 0;
}

static int fat_remove(struct inode *inode) {
	struct fat_file_info *fi;
	int res;
	fi = inode->i_data;
	res = fat_unlike_file(fi, NULL, (uint8_t*) fat_sector_buff);
	if (res == 0)
		fat_file_free(fi);

	return res;
}

/* Declaration of operations */
struct inode_operations fat_iops = {
	.create   = fat_create,
	.lookup   = fat_ilookup,
	.remove   = fat_remove,
	//.mkdir    = NULL,
	//.rmdir    = NULL,
	.iterate  = fat_iterate,
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

/* @brief Initializing fat super_block
 * @param sb  Structure to be initialized
 * @param dev Storage device
 *
 * @return Negative error code
 */
static int fat_fill_sb(struct super_block *sb, struct block_dev *dev) {
	struct fat_fs_info *fsi;
	uint32_t pstart, psize;
	uint8_t pactive, ptype;
	assert(sb);
	assert(dev);

	fsi = fat_fs_alloc();
	*fsi = (struct fat_fs_info) {
		.bdev = dev,
	};
	sb->sb_data = fsi;
	sb->sb_iops = &fat_iops;
	sb->sb_fops = &fat_fops;

	pstart = fat_get_ptn_start(dev, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) {
		return -1;
	}

	if (fat_get_volinfo(dev, &fsi->vi, pstart)) {
		return -1;
	}

	return 0;
}

static struct dumb_fs_driver dfs_fat_driver = {
	.name    = "vfat",
	.fill_sb = fat_fill_sb,
};

ARRAY_SPREAD_DECLARE(struct dumb_fs_driver *, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &dfs_fat_driver);

