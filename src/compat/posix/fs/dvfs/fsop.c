/**
 * @file
 *
 * @brief
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <fcntl.h>

#include <fs/dvfs.h>

int creat(const char *pathname, mode_t mode) {
	int rc;
	rc = open(pathname, O_CREAT | O_WRONLY | O_TRUNC | mode);
	DPRINTF(("creat(%s, %d ...) = %d\n", pathname, mode, rc));
	return rc;
}

int mkdir(const char *pathname, mode_t mode) {
	return creat(pathname, mode | O_DIRECTORY);
}

int remove(const char *pathname) {
	return dvfs_remove(pathname);
}

int unlink(const char *pathname) {
	return 0;
}

int rmdir(const char *pathname) {
	return 0;
}

int lstat(const char *path, struct stat *buf) {
	return 0;
}

int stat(const char *path, struct stat *buf) {
	struct lookup l;
	int res = dvfs_lookup(path, &l);
	struct dentry *d = l.item;

	if (res < 0)
		return res;

	*buf = (struct stat) {
		.st_dev     = d->d_sb->bdev->id,
		.st_ino     = d->d_inode->i_no,
		.st_nlink   = 1,
		.st_size    = d->d_inode->length,
		.st_blksize = 512,
	};

	return 0;
}

int truncate(const char *path, off_t length) {
	return 0;
}

int flock(int fd, int operation) {
	return 0;
}

