/**
* @file
* @brief
*
* @author Denis Deryugin
* @date 3 Apr 2015
*/

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <fs/inode.h>
#include <fs/dentry.h>
#include <kernel/task/resource/vfs.h>
#include <util/atomic_rmw.h>
#include <util/log.h>

/**
* @brief POSIX-compatible function changing process working directory
*
* @param path Path to new working directory
*
* @return On success, 0 returned, otherwise -1 returned and ERRNO is set
*	 appropriately.
*/
int chdir(const char *path) {
	struct lookup l = { NULL, NULL };
	int err;
	char new_pwd[PATH_MAX - 1];
	struct task_vfs *t;
	struct dentry *old;

	if (path == NULL) {
		SET_ERRNO(ENOENT);
		return -1;
	}

	if ((err = dvfs_lookup(path, &l))) {
		return SET_ERRNO(-err);
	}

	if (l.item == NULL) {
		return SET_ERRNO(ENOENT);
	}

	if (!(l.item->flags & S_IFDIR)) {
		dentry_ref_dec(l.item);
		return SET_ERRNO(ENOTDIR);
	}

	dentry_full_path(l.item, new_pwd);

	if ((t = task_self_resource_vfs()) == NULL) {
		dentry_ref_dec(l.item);
		log_error("task VFS structure is NULL");
		return SET_ERRNO(EIO);
	}

	if (-1 == setenv("PWD", new_pwd, 1)) {
		dentry_ref_dec(l.item);
		assert(errno == ENOMEM);
		return SET_ERRNO(ENAMETOOLONG);
	}

	/* The lookup's reference becomes the working directory's, so there is
	 * no moment at which reclaim could take the directory. */
	old = atomic_rmw_exchange(&t->pwd, l.item, __ATOMIC_ACQ_REL);
	if (old) {
		dentry_ref_dec(old);
	}

	return 0;
}
