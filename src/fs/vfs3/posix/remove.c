/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <vfs/core.h>

int remove(const char *path) {
	int err;

	vfs_inode_ops_lock();
	err = vfs_path_remove(path);
	vfs_inode_ops_unlock();

	if (err) {
		return SET_ERRNO(-err);
	}

	return 0;
}
