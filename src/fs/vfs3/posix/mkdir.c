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

int mkdir(const char *path, mode_t mode) {
	int err;

	mode &= ~umask(0);
	mode &= S_IRWXA;
	mode |= S_IFDIR;

	vfs_inode_ops_lock();
	err = vfs_path_mkdir(path, mode);
	vfs_inode_ops_unlock();

	if (err) {
		return SET_ERRNO(-err);
	}

	return 0;
}
