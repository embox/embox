/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

#include <fs/vfs.h>
#include <fs/path.h>
#include <fs/fs_driver.h>
#include <fs/kfsop.h>
#include <fs/perm.h>
#include <security/security.h>

static int create_new_node(struct node *parent, const char *name, mode_t mode) {
	struct node *node;
	struct fs_driver *drv;
	int retval = 0;

	node = vfs_create(parent, name, mode);
	if (!node) {
		return -ENOMEM;
	}

	/* check drv of parents */
	drv = parent->nas->fs->drv;
	if (!drv || !drv->fsop->create_node) {
		retval = -1;
		goto out;
	}

	retval = drv->fsop->create_node(parent, node);
	if (retval) {
		goto out;
	}

	return 0;

out:
	vfs_del_leaf(node);
	return retval;
}

int kmkdir(struct node *root_node, const char *pathname, mode_t mode) {
	struct node *node;
	const char *lastpath, *ch;
	int res;

	if (0 == (res = fs_perm_lookup(root_node, pathname, &lastpath, &node))) {
		errno = EBUSY;
		return -1;
	} else if (-EACCES == res) {
		errno = EACCES;
		return -1;
	}

	if (NULL != (ch = strchr(lastpath, '/'))
			&& NULL != path_next(ch, NULL)) {
		errno = ENOENT;
		return -1;
	}

	if (0 != fs_perm_check(node, FS_MAY_WRITE)) {
		errno = EACCES;
		return -1;
	}

	if (0 != (res = security_node_create(node, S_IFDIR | mode))) {
		errno = -res;
		return -1;
	}


	if (0 != (res = create_new_node(node, lastpath, S_IFDIR | mode))) {
		errno = -res;
		return -1;
	}

	return 0;
}

int kremove(const char *pathname) {
	node_t *node;
	struct nas *nas;
	struct fs_driver *drv;
	int res;

	if (0 != (res = fs_perm_lookup(vfs_get_root(), pathname, NULL, &node))) {
		errno = -res;
		return -1;
	}

	nas = node->nas;
	drv = nas->fs->drv;
	if (NULL == drv->fsop->delete_node) {
		errno = EPERM;
		return -1;
	}

	if (node_is_directory(node)) {
		return rmdir(pathname);
	}
	else {
		return unlink(pathname);
	}
}

int kunlink(const char *pathname) {
	node_t *node;
	struct fs_driver *drv;
	int res;

	if (0 != fs_perm_lookup(vfs_get_root(), pathname, NULL, &node)) {
		errno = EACCES;
		return -1;
	}

	if (0 != fs_perm_check(node_parent(node), FS_MAY_WRITE)) {
		errno = EACCES;
		return -1;
	}

	if (0 != (res = security_node_delete(node_parent(node), node))) {
		return res;
	}

	drv = node->nas->fs->drv;

	if (NULL == drv->fsop->delete_node) {
		errno = EPERM;
		return -1;
	}

	if (0 != (res = drv->fsop->delete_node(node))) {
		errno = -res;
		return -1;
	}

	/*vfs_del_leaf(node);*/

	return 0;

}

int krmdir(const char *pathname) {
	node_t *node;
	struct fs_driver *drv;
	int res;

	if (0 != (res = fs_perm_lookup(vfs_get_root(), pathname, NULL, &node))) {
		errno = -res;
		return -1;
	}

	if (0 != (res = fs_perm_check(node, FS_MAY_WRITE))) {
		errno = EACCES;
		return -1;
	}

	if (0 != (res = security_node_delete(node_parent(node), node))) {
		return res;
	}

	drv = node->nas->fs->drv;

	if (NULL == drv->fsop->delete_node) {
		errno = EPERM;
		return -1;
	}

	if (0 != (res = drv->fsop->delete_node(node))) {
		errno = -res;
		return -1;
	}

	/*vfs_del_leaf(node);*/

	return 0;

}

int klstat(const char *path, struct stat *buf) {
	node_t *node;
	int res;

	if (0 != (res = fs_perm_lookup(vfs_get_root(), path, NULL, &node))) {
		errno = -res;
		return -1;
	}

	kfile_fill_stat(node, buf);

	return 0;
}

int kformat(const char *pathname, const char *fs_type) {
	node_t *node;
	struct fs_driver *drv;
	int res;

	if (0 != fs_type) {
		drv = fs_driver_find_drv((const char *) fs_type);
		if (NULL == drv) {
			return -EINVAL;
		}
		if (NULL == drv->fsop->format) {
			return  -ENOSYS;
		}
	}
	else {
		return -EINVAL;
	}

	if (0 != (res = fs_perm_lookup(vfs_get_root(), pathname, NULL, &node))) {
		errno = res == -ENOENT ? ENODEV : -res;
		return -1;
	}

	if (0 != (res = fs_perm_check(node, FS_MAY_WRITE))) {
		errno = EACCES;
		return -1;
	}

	return drv->fsop->format(node);
}

int kmount(const char *dev, const char *dir, const char *fs_type) {
	struct node *dev_node, *dir_node;
	struct fs_driver *drv;
	const char *lastpath;
	int res;

	if (!fs_type) {
		return -EINVAL;
	}

	drv = fs_driver_find_drv(fs_type);
	if (!drv) {
		return -EINVAL;
	}
	if (!drv->fsop->mount) {
		return  -ENOSYS;
	}

	if (0 == strcmp(fs_type, "nfs")) {
		dev_node = (node_t *) dev;
		goto skip_dev_lookup;
	}

	if (0 != (res = fs_perm_lookup(vfs_get_root(), dev, &lastpath, &dev_node))) {
		errno = res == -ENOENT ? ENODEV : -res;
		return -1;
	}

	if (0 != fs_perm_check(dev_node, FS_MAY_READ | FS_MAY_EXEC)) {
		errno = EACCES;
		return -1;
	}

skip_dev_lookup:
	/* find directory */
	if (0 != (res = fs_perm_lookup(vfs_get_root(), dir, &lastpath, &dir_node))) {
		errno = -res;
		return -1;
#if 0
		if (res != -ENOENT) {
			errno = -res;
		}

		if (0 != (res = kmkdir(dir_node, lastpath, 0755))) {
			return res;
		}
#endif
	}

	if (0 != (res = security_mount(dev_node, dir_node))) {
		return res;
	}

	return drv->fsop->mount(dev_node, dir_node);
}

int kumount(const char *dir) {
	struct node *dir_node;
	struct fs_driver *drv;
	const char *lastpath;
	int res;

	/* find directory */
	if (0 != (res = fs_perm_lookup(vfs_get_root(), dir, &lastpath, &dir_node))) {
		errno = -res;
		return -1;
	}

	/* TODO fs_perm_check(dir_node, FS_MAY_XXX) */

	drv = dir_node->nas->fs->drv;

	if (!drv) {
		return -EINVAL;
	}
	if (!drv->fsop->umount) {
		return  -ENOSYS;
	}

	if (0 != (res = security_umount(dir_node))) {
		return res;
	}

	return drv->fsop->umount(dir_node);
}
