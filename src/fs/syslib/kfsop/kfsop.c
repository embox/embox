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

static int create_new_node(struct path *parent, const char *name, mode_t mode) {
	struct path node;
	int retval = 0;

	if(NULL == parent->node->i_sb) {
		return -EINVAL;
	}

	if (0 != vfs_create(parent, name, mode, &node)) {
		return -ENOMEM;
	}

	if ((mode & VFS_DIR_VIRTUAL) && S_ISDIR(mode)) {
		node.node->i_sb = parent->node->i_sb;
	} else {
		struct super_block *sb = parent->node->i_sb;

		if (!sb || !sb->sb_iops || !sb->sb_iops->ino_create) {
			retval = -ENOSYS;/* FIXME EPERM ?*/
			goto out;
		}

		retval = sb->sb_iops->ino_create(node.node, parent->node, node.node->i_mode);
		if (retval) {
			goto out;
		}
	}
	/* XXX it's here and not in vfs since vfs node associated with drive after
 	 * creating. security may call driver dependent features, like setting
	 * xattr
	 */
	security_node_cred_fill(node.node);
	return 0;
out:
	vfs_del_leaf(node.node);
	return retval;
}

int kmkdir(const char *pathname, mode_t mode) {
	struct path node;
	const char *lastpath, *ch;
	int res;

	if (-ENOENT != (res = fs_perm_lookup(pathname, &lastpath, &node))) {
		errno = EEXIST;
		return -1;
	}

	if (NULL != (ch = strchr(lastpath, '/'))
			&& NULL != path_next(ch, NULL)) {
		errno = ENOENT;
		return -1;
	}

	if_mounted_follow_down(&node);

	if (0 != fs_perm_check(node.node, S_IWOTH)) {
		errno = EACCES;
		return -1;
	}

	if (0 != (res = security_node_create(node.node, S_IFDIR | mode))) {
		errno = -res;
		return -1;
	}

	if (0 != (res = create_new_node(&node, lastpath, S_IFDIR | mode))) {
		errno = -res;
		return -1;
	}

	return 0;
}

int kcreat(struct path *dir_path, const char *path, mode_t mode, struct path *child) {
	struct super_block *sb;
	int ret;

	assert(dir_path->node);

	path = path_next(path, NULL);

	if (!path) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	if (NULL != strchr(path, '/')) {
		SET_ERRNO(ENOENT);
		return -1;
	}

	if (0 != (fs_perm_check(dir_path->node, S_IWOTH))) {
		SET_ERRNO(EACCES);
		return -1;
	}

	if (0 != (ret = security_node_create(dir_path->node, S_IFREG | mode))) {
		SET_ERRNO(-ret);
		return -1;
	}

	if (0 != vfs_create(dir_path, path, S_IFREG | mode, child)) {
		SET_ERRNO(ENOMEM);
		return -1;
	}

	child->mnt_desc = dir_path->mnt_desc;

	if(!dir_path->node || !dir_path->node->i_sb) {
		SET_ERRNO(EBADF);
		vfs_del_leaf(child->node);
		return -1;
	}

	sb = dir_path->node->i_sb;
	if (!sb || !sb->sb_iops || !sb->sb_iops->ino_create) {
		SET_ERRNO(EBADF);
		vfs_del_leaf(child->node);
		return -1;
	}
	
	ret = sb->sb_iops->ino_create(child->node, dir_path->node, child->node->i_mode);
	if (0 != ret) {
		SET_ERRNO(-ret);
		vfs_del_leaf(child->node);
		return -1;
	}

	/* XXX it's here and not in vfs since vfs node associated with drive after
 	 * creating. security may call driver dependent features, like setting
	 * xattr
	 */
	security_node_cred_fill(child->node);

	return 0;
}
