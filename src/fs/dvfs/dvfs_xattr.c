/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-26
 */

#include <errno.h>

#include <fs/dvfs.h>

int dvfs_xattr_get(const char *path, const char *name, char *value, size_t size) {
	struct lookup lu = {};
	struct inode *inode;
	int err;

	if ((err = dvfs_lookup(path, &lu)))
		return SET_ERRNO(-err);

	inode = lookup->item->d_inode;
	if (!inode->i_ops->ino_getxattr)
		return SET_ERRNO(ENOSUPP);

	return 0;
}
