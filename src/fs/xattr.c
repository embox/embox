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

#define RETURN_ERRNO(err) errno = err; return -1

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
	int err;
	struct node *node = vfs_find_node(path, NULL);

	if (0 > (err = check_xattr_access(node))) {
		RETURN_ERRNO(-err);
	}

	if (0 > (err = check_fsop(node, &fsop))) {
		RETURN_ERRNO(-err);
	}

	if (!fsop->getxattr) {
		RETURN_ERRNO(EINVAL);
	}

	if (0 > (err = fsop->getxattr(node, name, value, len))) {
		RETURN_ERRNO(-err);
	}

	return err;
}

int setxattr(const char *path, const char *name, const char *value, size_t len, int flags) {
	const struct fsop_desc *fsop;
	int err;
	struct node *node = vfs_find_node(path, NULL);

	if (0 > (err = check_xattr_access(node))) {
		RETURN_ERRNO(-err);
	}

	if (0 > (err = check_fsop(node, &fsop))) {
		RETURN_ERRNO(-err);
	}

	if (!fsop->setxattr) {
		RETURN_ERRNO(EINVAL);
	}

	if (0 > (err = fsop->setxattr(node, name, value, len, flags))) {
		RETURN_ERRNO(-err);
	}

	return err;
}

int listxattr(const char *path, char *list, size_t len) {
	const struct fsop_desc *fsop;
	int err;
	struct node *node = vfs_find_node(path, NULL);

	if (0 > (err = check_xattr_access(node))) {
		RETURN_ERRNO(-err);
	}

	if (0 > (err = check_fsop(node, &fsop))) {
		RETURN_ERRNO(-err);
	}

	if (!fsop->listxattr) {
		RETURN_ERRNO(EINVAL);
	}

	if (0 > (err = fsop->listxattr(node, list, len))) {
		RETURN_ERRNO(-err);
	}

	return err;
}

