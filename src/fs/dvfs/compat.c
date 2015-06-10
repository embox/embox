/**
 * @file
 * @brief Handle calls from old VFS
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-06-10
 */

#include <fs/dvfs.h>
#include <fs/mount.h>

int mount(char *dev, char *dir, char *fs_type) {
	struct block_dev *bdev;
	if (dev) {
		block_devs_init();
		bdev = block_dev_find(dev);
	}
	return dvfs_mount(bdev, dir, fs_type, 0);
}

int umount(char *dir) {
	return 0;
}
