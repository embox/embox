/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.01.2013
 */

#include <types.h>
#include <errno.h>

#include <fs/vfs.h>

#include <fs/xattr.h>

static int check_fsop(struct node *node, const struct fsop_desc **fsop) {
	if (!node) {
		return -ENOENT;
	}

	if (!node->nas) {
		return -EINVAL;
	}

	if (!node->nas->fs) {
		return -EINVAL;
	}

	if (!node->nas->fs->drv) {
		return -EINVAL;
	}

	if (!node->nas->fs->drv->fsop) {
		return -EINVAL;
	}

	*fsop = node->nas->fs->drv->fsop;
	return 0;
}

static int check_xattr_access(struct node *node) {
	return 0;
}

int getxattr(const char *path, const char *name, char *value, size_t len) {
	const struct fsop_desc *fsop;
	struct node *node;
	int err;

	node = vfs_lookup(NULL, path);
	if (!node) {
		return -ENOENT;
	}

	if ((err = check_xattr_access(node))) {
		return err;
	}

	if ((err = check_fsop(node, &fsop))) {
		return err;
	}

	if (!fsop->getxattr) {
		return -EINVAL;
	}

	return fsop->getxattr(node, name, value, len);
}

int setxattr(const char *path, const char *name, const char *value, size_t len, int flags) {
	const struct fsop_desc *fsop;
	struct node *node;
	int err;

	node = vfs_lookup(NULL, path);
	if (!node) {
		return -ENOENT;
	}

	if ((err = check_xattr_access(node))) {
		return err;
	}

	if ((err = check_fsop(node, &fsop))) {
		return err;
	}

	if (!fsop->setxattr) {
		return -EINVAL;
	}

	return fsop->setxattr(node, name, value, len, flags);
}

int listxattr(const char *path, char *list, size_t len) {
	const struct fsop_desc *fsop;
	struct node *node;
	int err;

	node = vfs_lookup(NULL, path);
	if (!node) {
		return -ENOENT;
	}

	if ((err = check_xattr_access(node))) {
		return err;
	}

	if ((err = check_fsop(node, &fsop))) {
		return err;
	}

	if (!fsop->listxattr) {
		return -EINVAL;
	}

	return fsop->listxattr(node, list, len);
}

