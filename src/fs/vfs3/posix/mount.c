/**
 * @brief
 *
 * @date 15.03.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <posix_errno.h>
#include <stdbool.h>
#include <sys/mount.h>

#include <util/err.h>
#include <vfs/core.h>

int mount(const char *source, const char *target, const char *filesystemtype,
    unsigned long mountflags, const void *data) {
	struct super_block *sb;
	struct inode mpt;
	int err;

	vfs_inode_ops_lock();
	{
		err = vfs_path_lookup(target, &mpt);
		if (err) {
			goto out_unlock;
		}

		sb = vfs_super_block_mount(filesystemtype, NULL, mountflags & MS_BIND);
		if ((err = ptr2err(sb))) {
			goto out_unlock;
		}

		vfs_mount_point_get_last(&mpt, &mpt);
		vfs_mount_point_add_next(&mpt, sb);

		vfs_inode_lock(&mpt);
	}
out_unlock:
	vfs_inode_ops_unlock();

	if (err) {
		return SET_ERRNO(-err);
	}

	return 0;
}

int umount(const char *target) {
	struct inode tmp;
	struct inode mpt;
	int err;

	vfs_inode_ops_lock();
	{
		err = vfs_path_lookup(target, &tmp);
		if (err) {
			goto out_unlock;
		}

		vfs_mount_point_get_last(&tmp, &tmp);

		if (!vfs_mount_point_get_prev(&tmp, &mpt)) {
			err = -EINVAL;
			goto out_unlock;
		}

		err = vfs_super_block_unmount(tmp.sb);
		if (err) {
			goto out_unlock;
		}

		vfs_inode_unlock(&mpt);
	}
out_unlock:
	vfs_inode_ops_unlock();

	if (err) {
		return SET_ERRNO(-err);
	}

	return 0;
}
