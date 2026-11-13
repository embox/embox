/**
 * @file
 * @brief getcwd() for DVFS: the path of the task's working directory.
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fs/dvfs.h>
#include <kernel/task/resource/vfs.h>

/* From the task's working directory, not $PWD: environment strings are 64
 * bytes, and a directory can be entered other than by chdir(). */
char *getcwd(char *buff, size_t size) {
	struct task_vfs *t;
	struct dentry *pwd = NULL;
	int res;

	if ((buff == NULL) || (size == 0)) {
		SET_ERRNO(EINVAL);
		return NULL;
	}

	/* Under the tree lock, so a chdir() on another thread cannot let reclaim
	 * free the old directory mid-walk. */
	t = task_self_resource_vfs();
	dvfs_lock();
	if (t && t->pwd) {
		pwd = t->pwd;
		dentry_ref_inc(pwd);
	}
	dvfs_unlock();

	if (pwd == NULL) {
		/* Before the first chdir: the root. */
		if (size < 2) {
			SET_ERRNO(ERANGE);
			return NULL;
		}
		strcpy(buff, "/");
		return buff;
	}

	res = dentry_full_path_n(pwd, buff, size);
	dentry_ref_dec(pwd);
	if (res) {
		SET_ERRNO(res == -ENAMETOOLONG ? ERANGE : -res);
		return NULL;
	}

	return buff;
}
