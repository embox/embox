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

		if (res == -1){
			/* End of directory */
			inode_free(node);
			return 0;
		}

		node->i_ops = dir->i_ops;

		node->i_sb = dir->i_sb;

		vfs_add_leaf(node, dir);

		if (S_ISDIR(node->i_mode)) {
			vfs_mount_walker(node);
		}
	} while (1);
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
		super_block_free(sb);
		//todo free root
		errno = -res;
		return -1;
	}

	return ENOERR;
}
