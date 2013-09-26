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
#include <sys/types.h>

#include <fs/vfs.h>
#include <fs/mount.h>
#include <fs/path.h>
#include <fs/fs_driver.h>
#include <fs/kfsop.h>
#include <fs/perm.h>

#include <security/security.h>
#include <sys/file.h>
#include <kernel/task/idx.h>
#include <kernel/spinlock.h>
#include <kernel/thread.h>

#include <mem/misc/pool.h>

#define MAX_FLOCK_QUANTITY OPTION_GET(NUMBER, flock_quantity)
POOL_DEF(flock_pool, flock_shared_t, MAX_FLOCK_QUANTITY);

static int create_new_node(struct node *parent, const char *name, mode_t mode) {
	struct node *node;
	struct fs_driver *drv;
	int retval = 0;

	node = vfs_create(parent, name, mode);
	if (!node) {
		return -ENOMEM;
	}

	if(NULL == parent->nas->fs) {
		return -EINVAL;
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

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), pathname, NULL, &node))) {
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

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), pathname, NULL, &node))) {
		errno = -res;
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

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), pathname, NULL, &node))) {
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

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), path, NULL, &node))) {
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

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), pathname, NULL, &node))) {
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
	struct node *dev_node, *dir_node, *parent;
	struct fs_driver *drv;
	const char *lastpath;
	int res;

	if (!fs_type) {
		errno = EINVAL;
		return -1;
	}

	drv = fs_driver_find_drv(fs_type);
	if (!drv) {
		errno = EINVAL;
		return -1;
	}
	if (!drv->fsop->mount) {
		errno = ENOSYS;
		return -1;
	}

	if ((0 == strcmp(fs_type, "nfs")) || (0 == strcmp(fs_type, "cifs"))) {
		dev_node = (node_t *) dev;
		goto skip_dev_lookup;
	}

	if (ENOERR != (res = fs_perm_lookup(vfs_get_leaf(), dev, &lastpath, &dev_node))) {
		errno = res == -ENOENT ? ENODEV : -res;
		return -1;
	}

	if (ENOERR != (res = fs_perm_check(dev_node, FS_MAY_READ | FS_MAY_EXEC))) {
		errno = EACCES;
		return -1;
	}

skip_dev_lookup:
	/* find directory */
	if (ENOERR != (res = fs_perm_lookup(vfs_get_leaf(), dir, &lastpath, &dir_node))) {
		errno = -res;
		return -1;
	}

	if (ENOERR != (res = security_mount(dev_node, dir_node))) {
		return res;
	}

	if(ENOERR != (res = mount_table_check(dir_node))) {
		return res;
	}

	if(ENOERR != (res = drv->fsop->mount(dev_node, dir_node))) {
		/*TODO restore previous fs type from parent dir */
		if(NULL != (parent = vfs_get_parent(dir_node))) {
			dir_node->nas->fs = parent->nas->fs;
			//dir_node->nas->fi->privdata = parent->nas->fi->privdata;
		}
		return res;

	}
	if(ENOERR != (res = mount_table_add(dir_node))) {
		drv->fsop->umount(dir_node);
		/*TODO restore previous fs type from parent dir */
		if(NULL != (parent = vfs_get_parent(dir_node))) {
			dir_node->nas->fs = parent->nas->fs;
			//dir_node->nas->fi->privdata = parent->nas->fi->privdata;
		}
		return res;
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

	oldfd = open(oldpath, O_RDONLY);
	if (-1 == oldfd) {
		return -1;
	}
	newfd = open(newpath, O_CREAT|O_WRONLY|O_TRUNC, 0);
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
	node_t *oldnode, *newnode, *diritem;
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
	rc = fs_perm_lookup(vfs_get_leaf(), (const char *) oldpathcopy,
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
	rc = fs_perm_lookup(vfs_get_leaf(), (const char *) newpathcopy,
			(const char **) &newpathcopy, &newnode);
	free(npc_free);
	if (0 == rc) {
		if (node_is_directory(newnode)) {
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
	if (node_is_directory(oldnode)) {
		rc = mkdir(newpath, oldnode->mode);
		if (-1 == rc) {
			return -1;
		}

		/**
		 * Following line should be here:
		 *  tree_foreach_children(diritem, (&oldnode->tree_link), tree_link) {
		 * But it's not working with it.
		 */
		link = tree_children_begin(&oldnode->tree_link);
		end_link = tree_children_end(&oldnode->tree_link);

		while (link != end_link) {
			diritem = tree_element(link, typeof(*diritem), tree_link);
			link = tree_children_next(link);

			if (0 != strcmp(".", diritem->name) &&
					0 != strcmp("..", diritem->name)) {
				diritemlen = strlen(diritem->name);
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
				strcat(oldpatharg, diritem->name);
				strcat(newpatharg, newpath);
				if (newpatharg[strlen(newpatharg) - 1] != '/') {
					strcat(newpatharg, "/");
				}
				strcat(newpatharg, diritem->name);

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
	rc = remove(oldpath);
	if (0 != rc) {
		return -1;
	}

	return ENOERR;
}

int kumount(const char *dir) {
	struct node *dir_node, *parent;
	struct fs_driver *drv;
	const char *lastpath;
	int res;

	/* find directory */
	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), dir, &lastpath, &dir_node))) {
		errno = -res;
		return -1;
	}

	/* check if dir not a root dir */
	if(-EBUSY != (res = mount_table_check(dir_node))) {
		errno = -EINVAL;
		return -1;
	}

	/*TODO check if it has a opened files */

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

	if(0 != (res = drv->fsop->umount(dir_node))) {
		return res;
	}

	mount_table_del(dir_node);

	/*restore previous fs type from parent dir */
	if(NULL != (parent = vfs_get_parent(dir_node))) {
		dir_node->nas->fs = parent->nas->fs;
		//dir_node->nas->fi->privdata = parent->nas->fi->privdata;
	}

	return 0;
}

static int flock_shared_get(flock_t *flock) {
	flock_shared_t *shlock;
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

static int flock_shared_put(flock_t *flock) {
	flock_shared_t *shlock;
	struct thread *current = thread_self();
	struct dlist_head *item, *next;

	dlist_foreach(item, next, &flock->shlock_holders) {
		shlock = dlist_entry(item, flock_shared_t, flock_link);
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
	flock_t *flock;
	struct idx_desc *idesc;
	struct mutex *exlock;
	spinlock_t *flock_guard;
	long *shlock_count;
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
	idesc = task_self_idx_get(fd);
	flock = &((struct file_desc *) idesc->data->fd_struct)->node->flock;
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
			}
		} else {
			/* We should unlock flock_guard here to avoid many processes
			 * waiting on spin lock at the enter to this critical section */
			spin_unlock(flock_guard);
			flock_exclusive_get(exlock);
		}
		spin_unlock(flock_guard);
	}

	/* Shared locking operation */
	if (LOCK_SH & operation) {
		spin_lock(flock_guard);
		/* If current thread is holder of exclusive lock then convert it to
		 * shared lock */
		if (exlock->lock_count > 0) {
			if (current == exlock->holder) {
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
		if (exlock->holder == current) {
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

	return -ENOERR;
}
