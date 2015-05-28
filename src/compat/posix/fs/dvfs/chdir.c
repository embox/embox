/**
 * @file
 * @brief
 *
 * @author Denis Deryugin
 * @date 3 Apr 2015
 */

#include <errno.h>

#include <fs/dvfs.h>
#include <kernel/task/resource/vfs.h>

/**
 * @brief POSIX-compatible function changing process working directory
 *
 * @param path Path to new working directory
 *
 * @return On success, 0 returned, otherwise -1 returned and ERRNO is set
 *         appropriately.
 */
int chdir(const char *path) {
	struct lookup l;
	dvfs_lookup(path, &l);
	if (l.item != NULL) {
		task_fs()->pwd = l.item;
		return 0;
	} else
		return SET_ERRNO(ENOENT);
}
