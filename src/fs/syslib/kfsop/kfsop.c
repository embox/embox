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

int kutime(const char *path,const struct utimbuf *times) {
	struct path node;
	int res;

	if (0 != (res = fs_perm_lookup(path, NULL, &node))) {
		return res;
	}

	return kfile_change_stat(node.node, times);
}

int kformat(const char *pathname, const char *fs_type) {
	struct path node;
	const struct fs_driver *drv;
	int res;
	struct block_dev *bdev;

	if (0 != fs_type) {
		drv = fs_driver_find((const char *) fs_type);
		if (NULL == drv) {
			return -EINVAL;
		}
		if (NULL == drv->format) {
			return  -ENOSYS;
		}
	}
	else {
		return -EINVAL;
	}

	if (0 != (res = fs_perm_lookup(pathname, NULL, &node))) {
		errno = res == -ENOENT ? ENODEV : -res;
		return -1;
	}

	if (0 != (res = fs_perm_check(node.node, S_IWOTH))) {
		errno = EACCES;
		return -1;
	}

	bdev = node.node->i_privdata;
	if (0 != (res = drv->format(bdev, NULL))) {
		errno = -res;
		return -1;
	}

	return 0;
}

static int vfs_mount_walker(struct inode *dir) {
	int res;
	struct dir_ctx dir_context = { };

	assert(dir);

	do {
		struct inode *node = inode_new(dir->i_sb);
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
	//const struct fs_driver *drv;
	struct super_block *sb;
	const char *lastpath;
	struct mount_descriptor *mnt_desc;
	int res;

	if (NULL == (sb = super_block_alloc(fs_type, source))) {
		return -ENOMEM;
	}
#if 0
	drv = sb->fs_drv;
	if (!drv->fsop->mount) {
		errno = ENOSYS;
		return -1;
	}
#endif
	if (ENOERR != (res = fs_perm_lookup(dest, &lastpath, &dir_node))) {
		errno = -res;
		return -1;
	}
#if 0
	if (ENOERR != (res = drv->fsop->mount(sb, sb->sb_root))) {
		//todo free root
		errno = -res;
		return -1;
	}
#endif
	if (sb->sb_root->i_ops && sb->sb_root->i_ops->ino_iterate) {
		/* If FS provides iterate handler, then we assume
		 * that we should use it to actually mount all these
		 * files */
		vfs_mount_walker(sb->sb_root);
	}

	mnt_desc = dir_node.mnt_desc;

	if (NULL == mount_table_add(&dir_node, mnt_desc, sb->sb_root, source)) {
		super_block_free(sb);
		//todo free root
		errno = -res;
		return -1;
	}

	return ENOERR;
}

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
	if (0 != (res = fs_perm_lookup(dir, &lastpath, &dir_node))) {
		errno = -res;
		return -1;
	}

	/* check if dir not a root dir */
//	if(-EBUSY != (res = mount_table_check(dir_node))) {
//		errno = -EINVAL;
//		return -1;
//	}

	/*TODO check if it has a opened files */

	/* TODO fs_perm_check(dir_node, FS_MAY_XXX) */

	if_mounted_follow_down(&dir_node);
	node = dir_node;

	drv = dir_node.node->i_sb->fs_drv;

	if (!drv) {
		return -EINVAL;
	}

	if (0 != (res = security_umount(dir_node.node))) {
		return res;
	}

	if (0 != (res = umount_walker(dir_node.node))) {
		return res;
	}

	super_block_free(dir_node.node->i_sb);
	//dir_node.node->i_sb->sb_root = NULL;

	if (dir_node.node != vfs_get_root()) {
		inode_free(dir_node.node);
	}

	mount_table_del(node.mnt_desc);

//	/*restore previous fs type from parent dir */
//	if(NULL != (parent = vfs_get_parent(dir_node))) {
//	}

	return 0;
}
