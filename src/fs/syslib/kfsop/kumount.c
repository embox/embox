/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <errno.h>

#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/perm.h>
#include <fs/vfs.h>
#include <fs/path.h>
#include <fs/mount.h>

#include <security/security.h>

#include <fs/file_desc.h>
#include <fs/kfsop.h>

static int umount_walker(struct inode *node) {
	struct inode *child;

	if (S_ISDIR(node->i_mode)) {
		while (NULL != (child = vfs_subtree_get_child_next(node, NULL))) {
			umount_walker(child);
		}
	}

	/* Return the filesystem data while the superblock exists: a held inode
	 * survives this walk and outlives the superblock. */
	inode_detach_fs(node);

	vfs_del_leaf(node);

	return 0;
}

int kumount(const char *dir) {
	struct path dir_node, node;
	const struct fs_driver *drv;
	const char *lastpath;
	struct super_block *umount_sb_before;
	int res;

	/* find directory */
	res = fs_perm_lookup(dir, &lastpath, &dir_node);
	if (0 != res) {
		errno = -res;
		return -1;
	}

	/* Refuse a path that is not a mount point: an inode mounted onto, or
	 * the root of a mounted volume. */
	if (!dir_node.node->mounted
	    && !(dir_node.mnt_desc
	        && dir_node.mnt_desc->mnt_root == dir_node.node)) {
		errno = EINVAL;
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

	/* Nothing below is undoable, so refuse while descriptors are open. The
	 * count and the seal are one step: no descriptor opens after it. */
	res = file_desc_seal_sb(dir_node.node->i_sb);
	if (res > 0) {
		errno = EBUSY;
		return -1;
	}

	/* Read before the walk: umount_walker() detaches every inode it visits. */
	umount_sb_before = dir_node.node->i_sb;

	res = umount_walker(dir_node.node);
	if (0 != res) {
		if (umount_sb_before) {
			umount_sb_before->sb_unmounting = 0;
		}
		return res;
	}

	/* super_block_free() frees sb_root, which dir_node.node is after
	 * if_mounted_follow_down(); do not release it again. */
	{
		struct super_block *umount_sb = umount_sb_before;
		struct inode *umount_root = umount_sb ? umount_sb->sb_root : NULL;

		super_block_free(umount_sb);

		if (dir_node.node != vfs_get_root()
		    && dir_node.node != umount_root) {
			/* Never inode_free() something that was
			 * reachable -- somebody may be standing on it. */
			inode_release(dir_node.node);
		}
	}

	mount_table_del(node.mnt_desc);

	return 0;
}
