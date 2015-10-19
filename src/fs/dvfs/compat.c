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

/**
 * @brief Mount given device on given directory
 *
 * @param dev Path to device
 * @param dir Path to mount point
 * @param fs_type Name of FS driver
 *
 * @return Negative error number or 0 if succeed
 */
int mount(char *dev, char *dir, char *fs_type) {
	struct block_dev *bdev;
	char *t;
	if (dev) {
		block_devs_init();
		/* Get bdev in smarter way? */
		t = strrchr(dev, '/');
		if (t)
			dev = t + 1;
		bdev = block_dev_find(dev);
	}
	return dvfs_mount(bdev, dir, fs_type, 0);
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
 */
int umount(char *dir) {
	extern struct dlist_head dentry_dlist;
	struct dentry *d, *dmount;
	char mount_path[DENTRY_NAME_LEN];

	dmount = NULL;

	dlist_foreach_entry(d, &dentry_dlist, d_lnk) {
		if (d->flags & DVFS_MOUNT_POINT) {
			if (dentry_full_path(d, mount_path)) {
				continue;
			}
			if (!strncmp(mount_path, dir, sizeof(mount_path))) {
				dmount = d;
				break;
			}
		}
	}
	if (!dmount)
		return -ENOENT;

	dvfs_umount(dmount);
	return 0;
}
