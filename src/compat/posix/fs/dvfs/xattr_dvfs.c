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

/* Resolve PATH to a held inode, or set errno and answer NULL. */
static struct dentry *xattr_lookup(const char *path) {
	struct lookup lookup = {};
	int err;

	if ((err = dvfs_lookup(path, &lookup))) {
		SET_ERRNO(-err);
		return NULL;
	}
	if (lookup.item == NULL) {
		SET_ERRNO(ENOENT);
		return NULL;
	}
	if (lookup.item->d_inode == NULL || lookup.item->d_inode->i_ops == NULL) {
		dentry_ref_dec(lookup.item);
		SET_ERRNO(ENOTSUP);
		return NULL;
	}
	return lookup.item;
}

int getxattr(const char *path, const char *name, char *value, size_t size) {
	struct dentry *d;
	struct inode *inode;
	int res;

	if (!(d = xattr_lookup(path))) {
		return -1;
	}
	inode = d->d_inode;

	res = inode->i_ops->ino_getxattr
	    ? inode->i_ops->ino_getxattr(inode, name, value, size)
	    : -ENOTSUP;

	dentry_ref_dec(d);

	return res < 0 ? SET_ERRNO(-res) : res;
}

int setxattr(const char *path, const char *name, const char *value, size_t size,
	       	int flags) {
	struct dentry *d;
	struct inode *inode;
	int res;

	if (!(d = xattr_lookup(path))) {
		return -1;
	}
	inode = d->d_inode;

	res = inode->i_ops->ino_setxattr
	    ? inode->i_ops->ino_setxattr(inode, name, value, size, flags)
	    : -ENOTSUP;

	dentry_ref_dec(d);

	return res < 0 ? SET_ERRNO(-res) : res;
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

