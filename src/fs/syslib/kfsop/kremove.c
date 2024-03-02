/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <utime.h>

#include <fs/dir_context.h>
#include <fs/fs_driver.h>
#include <fs/hlpr_path.h>
#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/inode_operation.h>
#include <fs/kfsop.h>
#include <fs/perm.h>
#include <fs/vfs.h>
#include <fs/path.h>

#include <security/security.h>

int kremove(const char *pathname) {
	struct path node;
	int res;

	if (0 != (res = fs_perm_lookup(pathname, NULL, &node))) {
		errno = -res;
		return -1;
	}

	if (S_ISDIR(node.node->i_mode)) {
		return krmdir(pathname);
	}
	else {
		return kunlink(pathname);
	}
}

int kunlink(const char *pathname) {
	struct path node, parent;
	struct super_block *sb;
	int res;

	if (0 != (res = fs_perm_lookup(pathname, NULL, &node))) {
		errno = -res;
		return -1;
	}

	vfs_get_parent(&node, &parent);
	if (0 != fs_perm_check(parent.node, S_IWOTH)) {
		errno = EACCES;
		return -1;
	}

	if (0 != (res = security_node_delete(parent.node, node.node))) {
		errno = -res;
		return -1;
	}

	sb = node.node->i_sb;

	if (NULL == sb->sb_iops->ino_remove) {
		errno = EPERM;
		return -1;
	}

	if (0 != (res = sb->sb_iops->ino_remove(parent.node, node.node))) {
		errno = -res;
		return -1;
	}

	vfs_del_leaf(node.node);

	return 0;

}

int krmdir(const char *pathname) {
	struct path node, parent, child;
	struct super_block *sb;
	int res;

	if (0 != (res = fs_perm_lookup(pathname, NULL, &node))) {
		errno = -res;
		return -1;
	}

	if (0 != (res = fs_perm_check(node.node, S_IWOTH))) {
		errno = EACCES;
		return -1;
	}

	/* Remove directory only if it's empty */
	if (0 == vfs_get_child_next(&node, NULL, &child)) {
		errno = ENOTEMPTY;
		return -1;
	}

	vfs_get_parent(&node, &parent);
	if (0 != (res = security_node_delete(parent.node, node.node))) {
		return res;
	}

	sb = node.node->i_sb;

	if (NULL == sb->sb_iops->ino_remove) {
		errno = EPERM;
		return -1;
	}

	if (0 != (res = sb->sb_iops->ino_remove(parent.node, node.node))) {
		errno = -res;
		return -1;
	}

	vfs_del_leaf(node.node);

	return 0;

}
