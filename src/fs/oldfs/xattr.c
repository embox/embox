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
#include <fs/super_block.h>
#include <fs/inode_operation.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/xattr.h>

static int check_inode(struct inode *node) {
	if (!node) {
		return -ENOENT;
	}

	if (!node->i_sb) {
		return -EINVAL;
	}

	if (!node->i_sb->fs_drv) {
		return -EINVAL;
	}


	return 0;
}

int kfile_xattr_get(struct inode *node, const char *name, char *value, size_t len) {
	int err;

	if (0 > (err = check_inode(node))) {
		return err;
	}

	if (!node->i_sb->sb_iops->ino_getxattr) {
		return -EINVAL;
	}
	
	err = node->i_sb->sb_iops->ino_getxattr(node, name, value, len);
	if (0 > err) {
		return err;
	}

	return err;
}

int kfile_xattr_set(struct inode *node, const char *name,
			const char *value, size_t len, int flags) {
	int err;

	if (0 > (err = check_inode(node))) {
		return err;
	}

	if (!node->i_sb->sb_iops->ino_setxattr) {
		return -EINVAL;
	}

	if (value == NULL || len == 0) {
		flags |= XATTR_REMOVE;
	}

	err = node->i_sb->sb_iops->ino_setxattr(node, name, value, len, flags);
	if (0 > err) {
		return err;
	}

	return err;
}

int kfile_xattr_list(struct inode *node, char *list, size_t len) {
	int err;

	if (0 > (err = check_inode(node))) {
		return err;
	}

	if (!node->i_sb->sb_iops->ino_listxattr) {
		return -EINVAL;
	}

	if (0 > (err = node->i_sb->sb_iops->ino_listxattr(node, list, len))) {
		return err;
	}

	return err;
}

