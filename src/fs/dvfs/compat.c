/**
 * @file
 * @brief Handle calls from old VFS
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-06-10
 */

#include <assert.h>
#include <errno.h>
#include <sys/stat.h>

#include <drivers/block_dev.h>
#include <fs/dvfs.h>
#include <fs/fsop.h>

int mkfs(const char *blk_name, const char *fs_type, char *fs_spec) {
	const struct fs_driver *drv = fs_driver_find(fs_type);
	struct block_dev *bdev;
	struct lookup lu = {};
	int err;

	if (!drv) {
		return -EINVAL;
	}

	if (!drv->format) {
		return -ENOSYS;
	}

	if ((err = dvfs_lookup(blk_name, &lu))) {
		return err;
	}

	if (!lu.item) {
		return -ENOENT;
	}

	if (!lu.item->d_inode || !lu.item->d_inode->i_privdata
	    || !S_ISBLK(lu.item->d_inode->i_mode)) {
		dentry_ref_dec(lu.item);
		return -ENOTBLK;
	}

	bdev = dev_module_to_bdev(lu.item->d_inode->i_privdata);

	/* The device is found; the dentry is not needed to format it. */
	dentry_ref_dec(lu.item);

	return drv->format(bdev, fs_spec);
}

/* The oldfs name for mkfs() (fs/fsop.h), for code written against oldfs.
 * -errno, as mkfs(). */
int format(const char *pathname, const char *fs_type) {
	return mkfs(pathname, fs_type, NULL);
}
