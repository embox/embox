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
	return dvfs_mount(dev, dir, fs_type, 0);
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
	struct lookup lu;

	dvfs_lookup(dir, &lu);
	if (!lu.item)
		return -ENOENT;

	if (!(lu.item->flags & DVFS_MOUNT_POINT))
		/* Not a mount point */
		return -EINVAL;

	dvfs_umount(lu.item);
	return 0;
}
