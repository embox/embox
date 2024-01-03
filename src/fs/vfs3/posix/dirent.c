/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <util/err.h>
#include <vfs/core.h>

#define DIR_POOL_SIZE OPTION_GET(NUMBER, dir_pool_size)

struct DIR_struct {
	struct inode inode;
	struct inode pos;
	struct dirent dirent;
	struct timespec mtime;
	bool end_of_dir;
};

POOL_DEF(dir_pool, DIR, DIR_POOL_SIZE);

static int __readdir(DIR *dir) {
	int err;

	do {
		if (vfs_inode_is_bad(&dir->pos)) {
			vfs_inode_get_mtime(&dir->inode, &dir->mtime);
		}
		else if (vfs_inode_is_modified(&dir->inode, &dir->mtime)) {
			return -ENOENT;
		}

		err = dir->inode.sb->drv->ops.readdir(&dir->inode, &dir->pos,
		    &dir->dirent);

		if (err) {
			dir->pos.ino = VFS_BAD_INO;
			dir->pos.sb = dir->inode.sb;
		}

	} while (err && vfs_mount_point_get_prev_bind(&dir->inode, &dir->inode));

	return err;
}

DIR *opendir(const char *path) {
	DIR *dir;
	int err;

	dir = pool_alloc(&dir_pool);
	if (!dir) {
		SET_ERRNO(ENFILE);
		return NULL;
	}

	vfs_inode_ops_lock();
	{
		err = vfs_path_lookup(path, &dir->inode);
		if (!err) {
			vfs_mount_point_get_last(&dir->inode, &dir->inode);
			vfs_inode_lock(&dir->inode);
		}
	}
	vfs_inode_ops_unlock();

	if (err) {
		pool_free(&dir_pool, dir);
		SET_ERRNO(-err);
		return NULL;
	}

	dir->pos.ino = VFS_BAD_INO;
	dir->pos.sb = dir->inode.sb;
	dir->end_of_dir = false;

	return dir;
}

int closedir(DIR *dir) {
	if (!dir->end_of_dir) {
		vfs_inode_unlock(&dir->inode);
	}
	pool_free(&dir_pool, dir);

	return 0;
}

struct dirent *readdir(DIR *dir) {
	struct inode old;
	int err;

	if (dir->end_of_dir) {
		SET_ERRNO(ENOENT);
		return NULL;
	}

	memcpy(&old, &dir->inode, sizeof(struct inode));

	vfs_inode_ops_lock();
	err = __readdir(dir);
	vfs_inode_ops_unlock();

	if (err) {
		dir->end_of_dir = true;
		vfs_inode_unlock(&dir->inode);
		SET_ERRNO(-err);
		return NULL;
	}

	if ((dir->inode.ino != old.ino) || (dir->inode.sb != old.sb)) {
		vfs_inode_lock(&dir->inode);
		vfs_inode_unlock(&old);
	}

	return &dir->dirent;
}
