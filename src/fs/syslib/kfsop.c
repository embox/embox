/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <utime.h>
#include <sys/file.h>

#include <drivers/device.h>

#include <fs/dir_context.h>
#include <fs/file_desc.h>
#include <fs/dentry.h>
#include <fs/fs_driver.h>
#include <fs/hlpr_path.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/kfsop.h>
#include <fs/mount.h>
#include <fs/perm.h>
#include <fs/vfs.h>

#include <security/security.h>


#include <kernel/spinlock.h>
#include <kernel/thread.h>

#include <mem/misc/pool.h>

#define MAX_FLOCK_QUANTITY OPTION_GET(NUMBER, flock_quantity)
POOL_DEF(flock_pool, struct flock_shared, MAX_FLOCK_QUANTITY);

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

int kremove(const char *pathname) {
	struct path node;
	struct super_block *sb;
	int res;

	if (0 != (res = fs_perm_lookup(pathname, NULL, &node))) {
		errno = -res;
		return -1;
	}

	sb = node.node->i_sb;
	if (NULL == sb->sb_iops->ino_remove) {
		errno = EPERM;
		return -1;
	}

	if (node_is_directory(node.node)) {
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

	if (0 != (res = sb->sb_iops->ino_remove(node.node))) {
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

	if (0 != (res = sb->sb_iops->ino_remove(node.node))) {
		errno = -res;
		return -1;
	}

	vfs_del_leaf(node.node);

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
			node_free(node);
			return 0;
		}

		node->i_ops = dir->i_ops;

		node->i_sb = dir->i_sb;

		vfs_add_leaf(node, dir);

		if (node_is_directory(node)) {
			vfs_mount_walker(node);
		}
	} while (1);
}

int kmount(const char *source, const char *dest, const char *fs_type) {
	struct path dir_node;
	const struct fs_driver *drv;
	struct super_block *sb;
	const char *lastpath;
	struct mount_descriptor *mnt_desc;
	int res;

	if (NULL == (sb = super_block_alloc(fs_type, source))) {
		return -ENOMEM;
	}

	drv = sb->fs_drv;
	if (!drv->fsop->mount) {
		errno = ENOSYS;
		return -1;
	}

	if (ENOERR != (res = fs_perm_lookup(dest, &lastpath, &dir_node))) {
		errno = -res;
		return -1;
	}

	if (ENOERR != (res = drv->fsop->mount(sb, sb->sb_root))) {
		//todo free root
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

	if (NULL == mount_table_add(&dir_node, mnt_desc, sb->sb_root, source)) {
		super_block_free(sb);
		//todo free root
		errno = -res;
		return -1;
	}

	return ENOERR;
}

/**
 * Simple util function to copy file in oldpath to newpath
 * @param Should be regular file to copy
 * @param Name of new copy
 * @return ENOERR if file successfully copied -1 and set errno in other way
 */
static int copy_file(const char *oldpath, const char *newpath) {
	int oldfd, newfd, rc;
	char buf[BUFSIZ];
	struct stat old_st;

	oldfd = open(oldpath, O_RDONLY);
	if (-1 == oldfd) {
		return -1;
	}

	if (-1 == fstat(oldfd, &old_st)) {
		return -1;
	}

	newfd = open(newpath, O_CREAT|O_WRONLY|O_TRUNC, old_st.st_mode & 0777);
	if (-1 == newfd) {
		return -1;
	}

	/* Copy bytes */
	while ((rc = read(oldfd, buf, sizeof(buf))) > 0) {
		if (write(newfd, buf, rc) <= 0) {
			SET_ERRNO(EIO);
			return -1;
		}
	}

	/* Close files and free memory*/
	rc = close(oldfd);
	if (0 != rc) {
		return -1;
	}
	rc = close(newfd);
	if (0 != rc) {
		return -1;
	}

	return ENOERR;
}

/**
 * Rename oldpath to newpath
 * @param Path to file or directory to rename
 * @param Destination path, could not be existent file
 * @return ENOERR if file successfully copied -1 and set errno in other way
 */
int krename(const char *oldpath, const char *newpath) {
	int rc, newpathlen, diritemlen;
	char *oldpathcopy, *newpathcopy;
	char *opc_free, *npc_free;
	char *name, *newpathbuf = NULL;
	char *newpatharg, *oldpatharg;
	struct path oldnode, newnode;
	struct inode *diritem;
	/* We use custom tree traversal while I can't
	 * get success with tree_foreach_children */
	struct tree_link *link, *end_link;

	if (PATH_MAX < strlen(oldpath) ||
			PATH_MAX < strlen(newpath)) {
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	/* Check if source file exists */
	oldpathcopy = strdup(oldpath);
	opc_free = oldpathcopy;
	rc = fs_perm_lookup((const char *) oldpathcopy,
			(const char **) &oldpathcopy, &oldnode);
	free(opc_free);
	if (0 != rc) {
		SET_ERRNO(-rc);
		return -1;
	}

	/* Check if destination file already exists or if directory were
	 * provided as destination path */
	newpathcopy = strdup(newpath);
	npc_free = newpathcopy;
	rc = fs_perm_lookup((const char *) newpathcopy,
			(const char **) &newpathcopy, &newnode);
	free(npc_free);
	if (0 == rc) {
		if (node_is_directory(newnode.node)) {
			/* Directory was passed as destination */
			name = strrchr(oldpath, '/') + 1;
			newpathlen = strlen(newpath) + strlen(name);
			if (newpathlen > PATH_MAX) {
				SET_ERRNO(ENAMETOOLONG);
				return -1;
			}
			newpathbuf = calloc(newpathlen + 2, sizeof(char));
			if (NULL == newpathbuf) {
				SET_ERRNO(ENOMEM);
				return -1;
			}
			strcat(newpathbuf, newpath);
			if (newpathbuf[strlen(newpathbuf) - 1] != '/') {
				strcat(newpathbuf, "/");
			}
			strcat(newpathbuf, name);
			newpath = newpathbuf;
		} else {
			SET_ERRNO(EINVAL);
			return -1;
		}
	}

	/**
	 * TODO:
	 * Here we should check if we move within one filesystem and don't copy
	 * data in such case. Instead of that just make new hardlink
	 * and remove old one.
	 */

	/* If oldpath is directory, copy it recursively */
	if (node_is_directory(oldnode.node)) {
		rc = kmkdir(newpath, oldnode.node->i_mode);
		if (-1 == rc) {
			return -1;
		}

		/**
		 * Following line should be here:
		 *  tree_foreach_children(diritem, (&oldnode->tree_link), tree_link) {
		 * But it's not working with it.
		 */
		link = tree_children_begin(&oldnode.node->tree_link);
		end_link = tree_children_end(&oldnode.node->tree_link);

		while (link != end_link) {
			char *node_name;

			diritem = tree_element(link, typeof(*diritem), tree_link);
			link = tree_children_next(link);

			node_name = inode_name(diritem);
			if (0 != strcmp(".", node_name) &&
					0 != strcmp("..", node_name)) {
				diritemlen = strlen(node_name);
				oldpatharg =
						calloc(strlen(oldpath) + diritemlen + 2, sizeof(char));
				newpatharg =
						calloc(strlen(newpath) + diritemlen + 2, sizeof(char));
				if (NULL == oldpatharg || NULL == newpatharg) {
					SET_ERRNO(ENOMEM);
					return -1;
				}

				strcat(oldpatharg, oldpath);
				if (oldpatharg[strlen(oldpatharg) - 1] != '/') {
					strcat(oldpatharg, "/");
				}
				strcat(oldpatharg, node_name);
				strcat(newpatharg, newpath);
				if (newpatharg[strlen(newpatharg) - 1] != '/') {
					strcat(newpatharg, "/");
				}
				strcat(newpatharg, node_name);

				/* Call itself recursively */
				if (-1 == krename(oldpatharg, newpatharg)) {
					return -1;
				}

				free(newpatharg);
				free(oldpatharg);
			}
		}
	/* Or copy file */
	} else {
		rc = copy_file(oldpath, newpath);
		if (-1 == rc) {
			return -1;
		}
	}

	if (NULL != newpathbuf) {
		free(newpathbuf);
	}

	/* Delete file in old path */
	rc = kremove(oldpath);
	if (0 != rc) {
		return -1;
	}

	return ENOERR;
}

static int umount_walker(struct inode *node) {
	struct inode *child;

	if (node_is_directory(node)) {
		while (NULL != (child = vfs_subtree_get_child_next(node, NULL))) {
			umount_walker(child);
		}
	}

	if (node->i_sb->fs_drv->fsop->umount_entry) {
		node->i_sb->fs_drv->fsop->umount_entry(node);
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

	dir_node.node->i_sb->sb_root = NULL;
	super_block_free(dir_node.node->i_sb);

	if (dir_node.node != vfs_get_root()) {
		node_free(dir_node.node);
	}

	mount_table_del(node.mnt_desc);

//	/*restore previous fs type from parent dir */
//	if(NULL != (parent = vfs_get_parent(dir_node))) {
//	}

	return 0;
}

static int flock_shared_get(struct node_flock *flock) {
	struct flock_shared *shlock;
	struct thread *current = thread_self();

	shlock = pool_alloc(&flock_pool);
	if (NULL == shlock) {
		return -ENOMEM;
	}
	dlist_add_next(dlist_head_init(&shlock->flock_link), &flock->shlock_holders);
	shlock->holder = current;
	flock->shlock_count++;

	return -ENOERR;
}

static int flock_shared_put(struct node_flock *flock) {
	struct flock_shared *shlock;
	struct thread *current = thread_self();

	dlist_foreach_entry(shlock, &flock->shlock_holders, flock_link) {
		if (current == shlock->holder) {
			dlist_del(&shlock->flock_link);
			pool_free(&flock_pool, shlock);
			flock->shlock_count--;
		}
	}

	return -ENOERR;
}

static inline void flock_exclusive_get(struct mutex *exlock) {
	mutex_lock(exlock);
}

static inline int flock_exclusive_tryget(struct mutex *exlock) {
	return mutex_trylock(exlock);
}

static inline void flock_exclusive_put(struct mutex *exlock) {
	mutex_unlock(exlock);
}

/**
 * Apply advisory lock to specified fd
 * @param File descriptor number
 * @param Operation: one of LOCK_EX, LOCK_SH, LOCK_UN plus possible LOCK_NB
 * @return ENOERR if operation succeed or -1 and set errno in other way
 */
int kflock(int fd, int operation) {
	int rc;
	struct node_flock *flock;
	struct mutex *exlock;
	spinlock_t *flock_guard;
	long *shlock_count;
	struct file_desc *fdesc;
	struct thread *current = thread_self();

	/**
	 * Base algorithm:
	 * - Validate operation
	 * - Validate fd
	 * - Get lock pointer and other preparations
	 * - Determine operation (total 2 x 2 + 1 = 5)
	 *    1. Exclusive lock, blocking
	 *        - If shared block is acquired only by current thread convert it
	 *          to exclusive
	 *        - If shared or exclusive lock is acquired then block
	 *        - Else acquire exclusive lock
	 *    2. Exclusive lock, non-blocking
	 *        - The same as 1 but return EWOULDBLOCK instead of blocking
	 *    3. Shared lock, blocking
	 *        - If exclusive block is acquired by current thread then convert
	 *          it to shared
	 *        - If exclusive lock is acquired then block
	 *        - Else acquire shared lock
	 *    4. Shared lock, non-blocking
	 *        - The same as 3 but return EWOULDBLOCK instead of blocking
	 *    5. Unlock, blocking and non-blocking
	 *        - If any lock is acquired by current thread then remove it
	 *          blocking and non-blocking the same because mutex_unlock
	 *          never blocks thread in current implementation
	 */

	/* Validate operation */
	if (((LOCK_EX | LOCK_SH | LOCK_UN) & operation) != LOCK_EX && \
			((LOCK_EX | LOCK_SH | LOCK_UN) & operation) != LOCK_SH && \
			((LOCK_EX | LOCK_SH | LOCK_UN) & operation) != LOCK_UN)
		return -EINVAL;

	/* - Find locks and other properties for provided file descriptor number
	 * - fd is validated inside task_self_idx_get */
	fdesc = file_desc_get(fd);
	flock = &(fdesc)->f_inode->flock;
	exlock = &flock->exlock;
	shlock_count = &flock->shlock_count;
	flock_guard = &flock->flock_guard;

	/* Exclusive locking operation */
	if (LOCK_EX & operation) {
		spin_lock(flock_guard);
		/* If shared lock is acquired by any thread then free up our locks and
		 * try to acquire exclusive one */
		if (*shlock_count > 0) {
			/* We can hold only one type of lock at the moment */
			assert(0 == exlock->lock_count);
			flock_shared_put(flock);
		}
		if (LOCK_NB & operation) {
			if (-EBUSY == flock_exclusive_tryget(exlock)) {
				spin_unlock(flock_guard);
				SET_ERRNO(EWOULDBLOCK);
				return -1;
			} else {
				spin_unlock(flock_guard);
			}
		} else {
			/* We should unlock flock_guard here to avoid many processes
			 * waiting on spin lock at the enter to this critical section */
			spin_unlock(flock_guard);
			flock_exclusive_get(exlock);
		}
	}

	/* Shared locking operation */
	if (LOCK_SH & operation) {
		spin_lock(flock_guard);
		/* If current thread is holder of exclusive lock then convert it to
		 * shared lock */
		if (exlock->lock_count > 0) {
			if (&current->schedee == exlock->holder) {
				/* Again no two different types of lock can be held
				 * simultaneously */
				assert(0 == *shlock_count);
				/* Exclusive lock can be acquired many times by one thread
				 * that is because of current implementation of mutexes, so
				 * if we converting lock to shared we need to free all of them
				 */
				while (exlock->lock_count != 0) flock_exclusive_put(exlock);
			} else {
				if (LOCK_NB & operation) {
					if (-EBUSY == flock_exclusive_tryget(exlock)) {
						spin_unlock(flock_guard);
						SET_ERRNO(EWOULDBLOCK);
						return -1;
					}
				} else {
					/* The same as for exclusive lock we are unlocking
					 * flock_guard to avoid many processes waiting on
					 * spin lock at the enter to this critical section */
					spin_unlock(flock_guard);
					flock_exclusive_get(exlock);
					spin_lock(flock_guard);
					flock_exclusive_put(exlock);
				}
			}
		}
		/* Acquire shared lock */
		rc = flock_shared_get(flock);
		if (-ENOERR != rc) {
			return rc;
		}
		spin_unlock(flock_guard);
	}

	/* Unlock operation */
	if (LOCK_UN & operation) {
		spin_lock(flock_guard);
		/* Handle exclusive lock free */
		if (exlock->holder == &current->schedee) {
			assert(0 == *shlock_count);
			/* mutex_unlock can't block the thread
			 * so nothing need for LOCK_NB */
			while (exlock->lock_count != 0) flock_exclusive_put(exlock);
		}
		/* Handle shared lock free */
		if (*shlock_count > 0) {
			assert(0 == exlock->lock_count);
			flock_shared_put(flock);
		}
		spin_unlock(flock_guard);
	}

	return ENOERR;
}
