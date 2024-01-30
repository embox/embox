/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.01.2013
 */

#include <stddef.h>
#include <errno.h>
#include <security/security.h>

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/xattr.h>

static int check_fsop(struct inode *node, const struct fsop_desc **fsop) {
	if (!node) {
		return -ENOENT;
	}

	if (!node->nas) {
		return -EINVAL;
	}

	if (!node->i_sb) {
		return -EINVAL;
	}

	if (!node->i_sb->fs_drv) {
		return -EINVAL;
	}

	if (!node->i_sb->fs_drv->fsop) {
		return -EINVAL;
	}

	*fsop = node->i_sb->fs_drv->fsop;
	return 0;
}

int kfile_xattr_get(struct inode *node, const char *name, char *value, size_t len) {
	const struct fsop_desc *fsop;
	int err;

	if (0 > (err = check_fsop(node, &fsop))) {
		return err;
	}

	if (!fsop->getxattr) {
		return -EINVAL;
	}

	if (0 > (err = fsop->getxattr(node, name, value, len))) {
		return err;
	}

	return err;
}

int kfile_xattr_set(struct inode *node, const char *name,
			const char *value, size_t len, int flags) {
	const struct fsop_desc *fsop;
	int err;

	if (0 > (err = check_fsop(node, &fsop))) {
		return err;
	}

	if (!fsop->setxattr) {
		return -EINVAL;
	}

	if (value == NULL || len == 0) {
		flags |= XATTR_REMOVE;
	}

	if (0 > (err = fsop->setxattr(node, name, value, len, flags))) {
		return err;
	}

	return err;
}

int kfile_xattr_list(struct inode *node, char *list, size_t len) {
	const struct fsop_desc *fsop;
	int err;

	if (0 > (err = check_fsop(node, &fsop))) {
		return err;
	}

	if (!fsop->listxattr) {
		return -EINVAL;
	}

	if (0 > (err = fsop->listxattr(node, list, len))) {
		return err;
	}

	return err;
}

