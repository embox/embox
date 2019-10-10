/**
 * @file
 * @brief
 *
 * @author Denis Deryugin
 * @date 3 Apr 2015
 */

#include <errno.h>
#include <stdlib.h>

#include <fs/dvfs.h>
#include <kernel/task/resource/vfs.h>
#include <util/log.h>

/**
 * @brief POSIX-compatible function changing process working directory
 *
 * @param path Path to new working directory
 *
 * @return On success, 0 returned, otherwise -1 returned and ERRNO is set
 *         appropriately.
 */
int chdir(const char *path) {
	struct lookup l = { NULL, NULL };
	int err;

	if ((err = dvfs_lookup(path, &l))) {
		return SET_ERRNO(-err);
	}
	if (l.item != NULL) {
		char new_pwd[PATH_MAX - 1];
		struct task_vfs *t = NULL;

		if (!(l.item->flags & S_IFDIR)) {
			return SET_ERRNO(ENOTDIR);
		}

		dentry_full_path(l.item, new_pwd);

		if (-1 == setenv("PWD", new_pwd, 1)) {
			SET_ERRNO(ENAMETOOLONG);
			return -1;
		}

		if ((t = task_fs()) == NULL) {
			log_error("task VFS structure is NULL");
			return SET_ERRNO(EIO);
		}

		if (t->pwd != l.item) {
			dentry_ref_dec(t->pwd);
			t->pwd = l.item;
			dentry_ref_inc(t->pwd);
		}

		return 0;
	} else {
		return SET_ERRNO(ENOENT);
	}
}
