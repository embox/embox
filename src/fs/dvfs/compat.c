/**
 * @file
 * @brief Handle calls from old VFS
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-06-10
 */

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

int umount(char *dir) {
	return 0;
}
