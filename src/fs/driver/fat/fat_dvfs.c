/**
 * @file
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
#include <util/math.h>

extern uint8_t fat_sector_buff[FAT_MAX_SECTOR_SIZE];

extern struct file_operations fat_fops;

/**
* @brief Read related dir entries into dir buffer
*
* @param fsi Used to determine bdev and fat type (12/16/32)
* @param di  Pointer to dirinfo structure
*
* @return Negative error number
* @retval 0 Success
*/
static inline int read_dir_buf(struct fat_fs_info *fsi, struct dirinfo *di) {
	struct volinfo *vi = &fsi->vi;
	if (vi->filesystem == FAT32)
		return fat_read_sector(fsi, fat_sector_buff,
		                       vi->dataarea + (di->currentcluster - 2) * vi->secperclus);
	else
		return fat_read_sector(fsi, fat_sector_buff, vi->rootdir);
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
	struct dirinfo *di, *new_di;
	struct dirent de;
	struct volinfo *vi;
	struct super_block *sb;
	uint8_t tmp;
	struct inode *node;
	char tmppath[PATH_MAX];
	char fat_name[12];
	int found = 0;

	assert(name);
	assert(dir->d_inode);
	assert(FILE_TYPE(dir->d_inode->flags, S_IFDIR));

	sb = dir->d_sb;
	di = dir->d_inode->i_data;
	vi = &((struct fat_fs_info*)sb->sb_data)->vi;

	assert(di);

	path_canonical_to_dir(fat_name, (char *) name);

	if (read_dir_buf(sb->sb_data, di) != DFS_OK)
		goto err_out;

	tmp = di->currententry;
	di->currententry = 0;
	while (!fat_get_next(sb->sb_data, di, &de)) {
		path_canonical_to_dir(tmppath, (char *) de.name);
		if (!memcmp(tmppath, fat_name, MSDOS_NAME)) {
			found = 1;
			break;
		}
	}

	if (!found)
		goto err_out;

	if (NULL == (node = dvfs_alloc_inode(sb)))
		goto err_out;

	if (de.attr & ATTR_DIRECTORY){
		if (NULL == (new_di = fat_dirinfo_alloc())) {
			dvfs_destroy_inode(node);
			goto err_out;
		}

		memset(new_di, 0, sizeof(struct dirinfo));
		new_di->p_scratch = fat_sector_buff;
		node->flags |= S_IFDIR;
		node->i_data = new_di;

		if (vi->filesystem == FAT32) {
			new_di->currentcluster = (uint32_t) de.startclus_l_l |
			  ((uint32_t) de.startclus_l_h) << 8 |
			  ((uint32_t) de.startclus_h_l) << 16 |
			  ((uint32_t) de.startclus_h_h) << 24;
		} else {
			new_di->currentcluster = (uint32_t) de.startclus_l_l |
			  ((uint32_t) de.startclus_l_h) << 8;
		}

		fi = &new_di->fi;
	} else {
		if (NULL == (fi   = fat_file_alloc())) {
			dvfs_destroy_inode(node);
			goto err_out;
		}
		node->i_data = fi;
	}

	*fi = (struct fat_file_info) {
		.fsi = sb->sb_data,
		.volinfo = vi,
	};

	if (di->currentcluster == 0) {
		fi->dirsector = vi->rootdir + di->currentsector;
	} else {
		fi->dirsector = vi->dataarea +
				((di->currentcluster - 2) *
				vi->secperclus) + di->currentsector;
	}
	fi->diroffset = di->currententry - 1;
	if (vi->filesystem == FAT32) {
		fi->cluster = (uint32_t) de.startclus_l_l |
		  ((uint32_t) de.startclus_l_h) << 8 |
		  ((uint32_t) de.startclus_h_l) << 16 |
		  ((uint32_t) de.startclus_h_h) << 24;
	} else {
		fi->cluster = (uint32_t) de.startclus_l_l |
		  ((uint32_t) de.startclus_l_h) << 8;
	}
	fi->firstcluster = fi->cluster;
	fi->filelen = (uint32_t) de.filesize_0 |
			      ((uint32_t) de.filesize_1) << 8 |
			      ((uint32_t) de.filesize_2) << 16 |
			      ((uint32_t) de.filesize_3) << 24;


	di->currententry = tmp;
	node->flags |= S_IRWXA;
	return node;
err_out:
	di->currententry = tmp;
	return NULL;
}

/* @brief Create new file or directory
 * @param i_new Inode to be filled
 * @param i_dir Inode realted to the parent
 * @param mode  Used to figure out file type
 *
 * @return Negative error code
 */
static int fat_create(struct inode *i_new, struct inode *i_dir, int mode) {
	int res;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	int fat_flags = 0;
	assert(i_new);
	assert(i_dir);
	/* TODO check file exists */

	fsi = i_dir->i_sb->sb_data;
	fi = fat_file_alloc();
	*fi = (struct fat_file_info) {
		.fsi     = fsi,
		.volinfo = &fsi->vi,
	};
	i_new->i_data = fi;

	fat_flags |= FILE_TYPE(mode, S_IFDIR);

	read_dir_buf(fsi, i_dir->i_data);
	res = fat_create_file(fi, i_dir->i_data, i_new->i_dentry->name, fat_flags);

	return res;
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
	fi->mode = O_RDWR; /* XXX */
	fat_write_file(fi, fat_sector_buff, buf, &res, size, &desc->f_inode->length);
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
	dirinfo = parent->i_data;
	//if (ctx->pos == 0)
		dirinfo->currententry = 0;
	read_dir_buf(fsi, dirinfo);

	while (((res = fat_get_next(fsi, dirinfo, &de)) ==  DFS_OK) || res == DFS_ALLOCNEW)
		if (de.name[0] == 0)
			continue;
		else
			break;

	switch (res) {
	case DFS_OK:
		strcpy(next->i_dentry->name, (char*) de.name);
		next->flags |= S_IRWXA;
		return 0;
	case DFS_EOF:
		//ctx->fs_ctx = 0;
		/* Fall through */
	default:
		return -1;
	}
}

static int fat_remove(struct inode *inode) {
	struct fat_file_info *fi;
	struct dirinfo *di;
	int res;

	if (FILE_TYPE(inode->flags, S_IFDIR)) {
		di = inode->i_data;
		res = fat_unlike_directory(&di->fi, NULL, (uint8_t*) fat_sector_buff);
		if (res == 0)
			fat_dirinfo_free(di);
	} else {
		fi = inode->i_data;
		res = fat_unlike_file(fi, NULL, (uint8_t*) fat_sector_buff);
		if (res == 0)
			fat_file_free(fi);
	}
	return res;
}

/* Declaration of operations */
struct inode_operations fat_iops = {
	.create   = fat_create,
	.lookup   = fat_ilookup,
	.remove   = fat_remove,
	.iterate  = fat_iterate,
};

struct file_operations fat_fops = {
	.close = fat_close,
	.write = fat_write,
	.read = fat_read,
};

struct super_block_operations fat_sbops = {
	.open_idesc = dvfs_file_open_idesc,
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
	sb->sb_ops  = &fat_sbops;

	pstart = fat_get_ptn_start(dev, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) {
		return -1;
	}

	if (fat_get_volinfo(dev, &fsi->vi, pstart)) {
		return -1;
	}

	return 0;
}

/**
* @brief Initialize dirinfo for root FAT directory
* @note  Should be called just after mounting FAT FS
*
* @param super_block Superblock of just
*
* @return Negative error code
* @retval 0 Success
*/
static int fat_mount_end(struct super_block *sb) {
	struct dirinfo *di;
	struct volinfo *vi;

	assert(sb->bdev->block_size <= FAT_MAX_SECTOR_SIZE);

	if (NULL == (di = fat_dirinfo_alloc()))
		return -ENOMEM;

	vi = &((struct fat_fs_info*)sb->sb_data)->vi;

	*di = (struct dirinfo) {
		.p_scratch = fat_sector_buff,
	};

	di->currentcluster = vi->filesystem == FAT32 ? vi->rootdir : 0;
	sb->root->d_inode->i_data = di;

	return 0;
}


/**
 * @brief Format given block device
 * @param dev Pointer to device
 * @note Should be block device
 *
 * @return Negative error code or 0 if succeed
 */
static int fat_format(void *dev, void *priv) {
	int fat_n = priv ? atoi((char*) priv) : 12;
	fat_create_partition(dev, fat_n);
	fat_root_dir_record(dev);

	return 0;
}

static struct dumb_fs_driver dfs_fat_driver = {
	.name      = "vfat",
	.fill_sb   = fat_fill_sb,
	.mount_end = fat_mount_end,
	.format    = fat_format,
};

ARRAY_SPREAD_DECLARE(struct dumb_fs_driver *, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &dfs_fat_driver);
