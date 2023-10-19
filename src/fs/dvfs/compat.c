/**
 * @file
 * @brief Handle calls from old VFS
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-06-10
 */

#include <errno.h>
#include <string.h>

#include <fs/dvfs.h>
#include <fs/mount.h>
#include <fs/fuse_module.h>

/**
 * @brief Mount given device on given directory
 *
 * @param source Path to device or another source of data
 * @param target Path to mount point
 * @param fs_type Name of FS driver
 *
 * @return Negative error number or 0 if succeed
 */
int mount(const char *source, const char *target,
	const char *fs_type) {
	struct fuse_module *fm;
	fm = fuse_module_lookup(fs_type);
	if (fm) {
		return fuse_module_mount(fm, source, target);
	}
	errno = dvfs_mount(source, target, fs_type, 0); /* Compatibility with old VFS */
	return errno;
}

/**
 * @brief Unmount given mount point
 *
 * @param dir Path to mount point
 *
 * @return Negative error number of zero if succeed
 * @retval 0 Success
 * @retval -ENOENT Mount point not found
 * @retval -EBUSY File system is in use
 * @retval -EINVAL File is not a mount point
 */
int umount(char *dir) {
	struct lookup lu = {};
	int err;

	if ((err = dvfs_lookup(dir, &lu))) {
		return err;
	}
	dentry_ref_dec(lu.item);

	if (!(lu.item->flags & DVFS_MOUNT_POINT))
		/* Not a mount point */
		return -EINVAL;

	return dvfs_umount(lu.item);
}

#include <drivers/block_dev.h>
#include <fs/fsop.h>

int mkfs(const char *blk_name, const char *fs_type, char *fs_spec) {
	const struct fs_driver *drv = fs_driver_find(fs_type);
	struct block_dev *bdev;
	struct lookup lu = {};
	int err;

	if (!drv) {
		return -EINVAL;
	}

	if ((err = dvfs_lookup(blk_name, &lu))) {
		return err;
	}

	if (!lu.item) {
		return -ENOENT;
	}

	assert(lu.item->d_inode);
	assert(lu.item->d_inode->i_data);

	bdev = dev_module_to_bdev(lu.item->d_inode->i_data);

	return drv->format(bdev, fs_spec);
}