/**
 * @brief
 *
 * @date 02.01.24
 * @author Aleksey Zhmulin
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <lib/cwalk.h>
#include <util/err.h>
#include <util/log.h>
#include <vfs/core.h>

struct idesc;

struct lookup {
	struct inode dir;
	struct inode file;
	char filename[NAME_MAX];
};

static int vfs_lookup_next(struct inode *dir_inode, struct cwk_segment *segment,
    struct lookup *lookup) {
	int err;

	if (!vfs_inode_is_directory(dir_inode)) {
		return -ENOTDIR;
	}

	vfs_mount_point_get_last(dir_inode, &lookup->dir);

	if (segment->size >= NAME_MAX) {
		return -ENAMETOOLONG;
	}

	memcpy(lookup->filename, segment->begin, segment->size);
	lookup->filename[segment->size] = '\0';

	do {
		if (!vfs_inode_is_available(&lookup->dir, O_RDONLY)) {
			err = -EACCES;
			continue;
		}
		err = lookup->dir.sb->drv->ops.lookup(&lookup->dir, lookup->filename,
		    &lookup->file);
	} while (err && vfs_mount_point_get_prev_bind(&lookup->dir, &lookup->dir));

	return err;
}

static int vfs_lookup(const char *path, struct lookup *lookup) {
	struct cwk_segment segment;
	const char *cwd;
	size_t len;
	int err;
	char normal_path[PATH_MAX];

	if (!path) {
		return -ENOENT;
	}

	if (cwk_path_is_absolute(path)) {
		len = cwk_path_normalize(path, normal_path, sizeof(normal_path));
	}
	else {
		cwd = getenv("PWD");
		if (!cwd) {
			cwd = "/";
		}
		if (cwk_path_is_relative(cwd)) {
			log_error("PWD is not an absolute path");
			return -ENOENT;
		}
		len = cwk_path_join(cwd, path, normal_path, sizeof(normal_path));
	}

	if (len >= PATH_MAX) {
		return -ENAMETOOLONG;
	}

	vfs_inode_get_root(&lookup->file);

	if (!cwk_path_get_first_segment(normal_path, &segment)) {
		lookup->dir.ino = VFS_BAD_INO;
		lookup->dir.sb = lookup->file.sb;
		return 0;
	}

	do {
		err = vfs_lookup_next(&lookup->file, &segment, lookup);
	} while (!err && cwk_path_get_next_segment(&segment));

	if ((err == -ENOENT) && !cwk_path_get_next_segment(&segment)) {
		lookup->file.ino = VFS_BAD_INO;
		lookup->file.sb = lookup->dir.sb;
		return 0;
	}

	return err;
}

int vfs_path_lookup(const char *path, struct inode *lookup) {
	struct lookup _lookup;
	int err;

	if ((err = vfs_lookup(path, &_lookup))) {
		return err;
	}

	if (vfs_inode_is_bad(&_lookup.file)) {
		return -ENOENT;
	}

	memcpy(lookup, &_lookup.file, sizeof(struct inode));

	return 0;
}

struct idesc *vfs_path_open(const char *path, int oflag, mode_t mode) {
	struct lookup lookup;
	int err;

	err = vfs_lookup(path, &lookup);
	if (err) {
		return err2ptr(-err);
	}

	if (!vfs_inode_is_bad(&lookup.file)) {
		if ((oflag & O_CREAT) && (oflag & O_EXCL)) {
			return err2ptr(EEXIST);
		}
		goto out;
	}

	if (!(oflag & O_CREAT)) {
		return err2ptr(ENOENT);
	}

	if (!vfs_inode_is_available(&lookup.dir, O_WRONLY)) {
		return err2ptr(EACCES);
	}

	if (!lookup.dir.sb->drv->ops.mkfile) {
		return err2ptr(ENOSUPP);
	}

	err = lookup.dir.sb->drv->ops.mkfile(&lookup.dir, lookup.filename,
	    &lookup.file);
	if (err) {
		return err2ptr(-err);
	}

	vfs_inode_update_mtime(&lookup.dir);
	vfs_inode_init(&lookup.file, (mode & S_IRWXA) | S_IFREG);

out:
	return lookup.file.sb->drv->ops.open(&lookup.file, oflag);
}

int vfs_path_mkdir(const char *path, mode_t mode) {
	struct lookup lookup;
	int err;

	err = vfs_lookup(path, &lookup);
	if (err) {
		return err;
	}

	if (!vfs_inode_is_bad(&lookup.file)) {
		return -EEXIST;
	}

	if (!vfs_inode_is_available(&lookup.dir, O_WRONLY)) {
		return -EACCES;
	}

	if (!lookup.dir.sb->drv->ops.mkdir) {
		return -ENOSUPP;
	}

	err = lookup.dir.sb->drv->ops.mkdir(&lookup.dir, lookup.filename,
	    &lookup.file);
	if (err) {
		return err;
	}

	vfs_inode_update_mtime(&lookup.dir);
	vfs_inode_init(&lookup.file, (mode & S_IRWXA) | S_IFDIR);

	return 0;
}

int vfs_path_remove(const char *path) {
	struct lookup lookup;
	int err;

	err = vfs_lookup(path, &lookup);
	if (err) {
		return err;
	}

	if (vfs_inode_is_bad(&lookup.file)) {
		return -ENOENT;
	}

	if (vfs_inode_is_mount_point(&lookup.file)
	    || vfs_mount_point_get_next(&lookup.file, &lookup.file)) {
		return -EBUSY;
	}

	if (!vfs_inode_is_available(&lookup.dir, O_WRONLY)) {
		return -EACCES;
	}

	if (!lookup.dir.sb->drv->ops.remove) {
		return -ENOSUPP;
	}

	return lookup.dir.sb->drv->ops.remove(&lookup.dir, &lookup.file);
}
