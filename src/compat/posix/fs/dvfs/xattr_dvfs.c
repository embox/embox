/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    3 Apr 2015
 */

#include <stddef.h>
#include <errno.h>

#include <fs/dvfs.h>

int getxattr(const char *path, const char *name, char *value, size_t size) {
	struct lookup lookup = {};
	struct inode *inode;
	int err;

	if ((err = dvfs_lookup(path, &lookup))) {
		return err;
	}
	inode = lookup.item->d_inode;

	if (inode->i_ops->ino_getxattr) {
		return inode->i_ops->ino_getxattr(inode, name, value, size);
	}

	return 0;
}

int setxattr(const char *path, const char *name, const char *value, size_t size,
	       	int flags) {
	struct lookup lookup = {};
	struct inode *inode;
	int err;

	if ((err = dvfs_lookup(path, &lookup))) {
		return err;
	}
	inode = lookup.item->d_inode;

	if (inode->i_ops->ino_setxattr) {
		return inode->i_ops->ino_setxattr(inode, name, value, size, flags);
	}

	return 0;
}

int listxattr(const char *path, char *list, size_t size) {
	return 0;
}

int fsetxattr(int fd, const char *name, const char *value, size_t size, int flags) {
	return 0;
}

int fgetxattr(int fd, const char *name, void *value, size_t size) {
	return 0;
}

int flistxattr(int fd, char *list, size_t size) {
	return 0;
}

