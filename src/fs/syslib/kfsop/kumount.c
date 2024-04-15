/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <errno.h>

#include <fs/inode.h>
#include <fs/perm.h>
#include <fs/vfs.h>
#include <fs/path.h>

#include <security/security.h>

#include <fs/kfsop.h>

static int umount_walker(struct inode *node) {
	struct inode *child;

	if (S_ISDIR(node->i_mode)) {
		while (NULL != (child = vfs_subtree_get_child_next(node, NULL))) {
			umount_walker(child);
		}
	}

	vfs_del_leaf(node);

	return 0;
}

int kumount(const char *dir) {
	struct path dir_node, node;
	const struct fs_driver *drv;
	const char *lastpath;
	int res;

	/* find directory */
	res = fs_perm_lookup(dir, &lastpath, &dir_node);
	if (0 != res) {
		errno = -res;
		return -1;
	}

	if_mounted_follow_down(&dir_node);
	node = dir_node;

	drv = dir_node.node->i_sb->fs_drv;

	if (!drv) {
		return -EINVAL;
	}
	res = security_umount(dir_node.node);
	if (0 != res) {
		return res;
	}

	res = umount_walker(dir_node.node);
	if (0 != res) {
		return res;
	}

	super_block_free(dir_node.node->i_sb);

	if (dir_node.node != vfs_get_root()) {
		inode_free(dir_node.node);
	}

	mount_table_del(node.mnt_desc);

	return 0;
}
