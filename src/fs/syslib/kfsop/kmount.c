/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <string.h>
#include <assert.h>

#include <fs/dir_context.h>
#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/inode_operation.h>
#include <fs/perm.h>
#include <fs/vfs.h>
#include <fs/path.h>

static int vfs_mount_walker(struct inode *dir) {
	int res;
	struct dir_ctx dir_context = { };

	assert(dir);

	do {
		struct inode *node;

		node = inode_new(dir->i_sb);
		if (NULL == node) {
			return -ENOMEM;
		}

		assert(dir->i_ops);
		res = dir->i_ops->ino_iterate(node,
				inode_name(node),
				dir,
				&dir_context);

		if (res != 0) {
			/* -1 is end of directory; other values are filesystem errors. */
			inode_free(node);
			return res == -1 ? 0 : res;
		}

		node->i_ops = dir->i_ops;

		node->i_sb = dir->i_sb;

		vfs_add_leaf(node, dir);

		if (S_ISDIR(node->i_mode)) {
			vfs_mount_walker(node);
		}
	} while (1);
}

/* Mirror of umount_walker() in kumount.c, for a mount that got as far as
 * walking the volume and then could not be registered. Same shape: children
 * first, then the node, filesystem gets its data back on the way. */
static void mount_unwalk(struct inode *node) {
	struct inode *child;

	if (S_ISDIR(node->i_mode)) {
		while (NULL != (child = vfs_subtree_get_child_next(node, NULL))) {
			mount_unwalk(child);
		}
	}

	inode_detach_fs(node);
	vfs_del_leaf(node);
}

int kmount(const char *source, const char *dest, const char *fs_type) {
	struct path dir_node;
	struct super_block *sb;
	const char *lastpath;
	struct mount_descriptor *mnt_desc;
	int res;

	sb = super_block_alloc(fs_type, source);
	if (NULL == sb) {
		return -ENOMEM;
	}

	if (!strcmp("/", dest)) {
		vfs_set_root(sb->sb_root);
	}

	res = fs_perm_lookup(dest, &lastpath, &dir_node);
	if (ENOERR != res) {
		/* Superblock already allocated; free it on failure. */
		super_block_free(sb);
		errno = -res;
		return -1;
	}

	if (sb->sb_root->i_ops && sb->sb_root->i_ops->ino_iterate) {
		/* If FS provides iterate handler, then we assume
		 * that we should use it to actually mount all these
		 * files */
		vfs_mount_walker(sb->sb_root);
	}

	mnt_desc = dir_node.mnt_desc;

	mnt_desc = mount_table_add(&dir_node, mnt_desc, sb->sb_root, source);
	if (NULL == mnt_desc) {
		/* Give back what the walker made. */
		mount_unwalk(sb->sb_root);
		super_block_free(sb);
		errno = EBUSY;
		return -1;
	}

	return ENOERR;
}
