/**
 * @file
 * @brief Implementation of FAT driver
 *
 * @date 28.03.2012
 * @author Andrey Gazukin
 */

#include <util/log.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <libgen.h>

#include <util/err.h>

#include <fs/file_desc.h>
#include <kernel/printk.h>
#include <fs/super_block.h>
#include <fs/fs_driver.h>
#include <fs/inode_operation.h>
#include <fs/inode.h>
#include <fs/fat.h>

#include <drivers/block_dev.h>

/* FIXME bdev_by_path is declared in dvfs.h and fs/mount.h */
extern struct block_dev *bdev_by_path(const char *source);

extern struct file_operations fat_fops;

extern int fat_clean_sb(struct super_block *sb);
extern int fat_destroy_inode(struct inode *inode);
extern int fat_create(struct inode *i_new, struct inode *i_dir, int mode);



#define DEFAULT_FAT_VERSION OPTION_GET(NUMBER, default_fat_version)
/**
 * @brief Format given block device
 * @param dev Pointer to device
 * @note Should be block device
 *
 * @return Negative error code or 0 if succeed
 */
int fat_format(struct block_dev *dev, void *priv) {
	int fat_n = priv ? atoi((char*) priv) : 0;
	struct block_dev *bdev = dev;

	assert(dev);

	if (!fat_n) {
		fat_n = DEFAULT_FAT_VERSION;
	}

	if (fat_n != 12 && fat_n != 16 && fat_n != 32) {
		log_error("Unsupported FAT version: FAT%d "
				"(FAT12/FAT16/FAT32 available)", fat_n);
		return -EINVAL;
	}

	fat_create_partition(bdev, fat_n);
	fat_root_dir_record(bdev);

	return 0;
}

extern struct inode_operations fat_iops;
extern struct super_block_operations fat_sbops;
extern struct file_operations fat_fops;
/* @brief Initializing fat super_block
 * @param sb  Structure to be initialized
 * @param dev Storage device
 *
 * @return Negative error code
 */
static int fat_fill_sb_unlocked(struct super_block *sb, const char *source) {
	struct fat_fs_info *fsi;
	struct block_dev *bdev;
	uint32_t pstart, psize;
	uint8_t pactive, ptype;
	struct dirinfo *di = NULL;
	int rc = 0;

	assert(sb);

	bdev = bdev_by_path(source);
	if (!bdev) {
		/* FAT always uses block device, so we can't fill superblock */
		return -ENOENT;
	}

	fsi = fat_fs_alloc();
	/* The pool runs out, and it runs out for a reason that has nothing to do
	 * with this call -- forty mounts that failed after allocating and never
	 * gave anything back will do it. The store below was unconditional: a data
	 * abort at FAR_EL1 = 0, three tests after the mounts that caused it. */
	if (fsi == NULL) {
		return -ENOMEM;
	}

	*fsi = (struct fat_fs_info) {
		.bdev = bdev,
		/* The first allocation after a mount pays one scan, and every one
		 * after it is a step. */
		.free_hint = 2,
	};
	sb->sb_data = fsi;
	sb->sb_iops = &fat_iops;
	sb->sb_fops = &fat_fops;
	sb->sb_ops  = &fat_sbops;
	sb->bdev    = bdev;

	/* Obtain pointer to first partition on first (only) unit */
	pstart = fat_get_ptn_start(bdev, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) {
		rc = -EINVAL;
		goto error;
	}

	if (fat_get_volinfo(bdev, &fsi->vi, pstart)) {
		rc = -EBUSY;
		goto error;
	}

	if (NULL == (di = fat_dirinfo_alloc())) {
		rc = -ENOMEM;
		goto error;
	}
	memset(di, 0, sizeof(struct dirinfo));
	di->p_scratch = fat_sector_buff;

	if (fat_open_rootdir(fsi, di)) {
		rc = -EBUSY;
		goto error;
	}

	di->fi.fsi = fsi;
	di->fi.volinfo = &fsi->vi;

	inode_priv_set(sb->sb_root, di);
	sb->sb_root->i_ops = &fat_iops;

	return 0;

error:
	if (di) {
		fat_dirinfo_free(di);
	}

	fat_fs_free(fsi);
	return rc;
}

/**
 * @brief Cleanup FS-specific stuff. No need to clean all files: VFS should
 * do it by itself
 *
 * @param sb Pointer to superblock
 *
 * @return Negative error code or 0 if succeed
 */
static int fat_clean_sb_unlocked(struct super_block *sb) {
	struct fat_fs_info *fsi;

	assert(sb);

	fsi = sb->sb_data;

	assert(fsi);

	fat_fs_free(fsi);

	/* Through destroy_inode rather than by hand, because it clears
	 * the inode's private pointer. super_block_free() calls inode_free() on
	 * this same root immediately afterwards, and inode_free now releases
	 * private data too -- freeing it here without clearing the pointer would
	 * put one dirinfo on the pool free list twice. */
	fat_destroy_inode(sb->sb_root);

	return 0;
}


/* Mount and unmount walk the directory tree through the same global scratch
 * buffer as everything else, and they can run while a second thread is
 * writing a file on another volume. */
int fat_fill_sb(struct super_block *sb, const char *source) {
	int res;

	fat_lock();
	res = fat_fill_sb_unlocked(sb, source);
	fat_unlock();
	return res;
}

int fat_clean_sb(struct super_block *sb) {
	int res;

	fat_lock();
	res = fat_clean_sb_unlocked(sb);
	fat_unlock();
	return res;
}

static const struct fs_driver fatfs_driver = {
	.name     = "vfat",
	.format = fat_format,
	.fill_sb  = fat_fill_sb,
	.clean_sb = fat_clean_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(fatfs_driver);
