/**
 * @file
 *
 * @data 12 oct. 2015
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <fcntl.h>

#include <fs/dvfs.h>

int stat(const char *path, struct stat *buf) {
	struct lookup l;
	int res = dvfs_lookup(path, &l);
	struct dentry *d = l.item;

	if (res < 0) {
		errno = -res;
		return -1;
	}

	assert(d);

	*buf = (struct stat) {
		.st_dev     = (d && d->d_sb && d->d_sb->bdev) ? d->d_sb->bdev->id : 0,
		.st_ino     = d->d_inode->i_no,
		.st_nlink   = 1,
		.st_size    = (d && d->d_inode) ? d->d_inode->length : 0,
		.st_blksize = 512,
		.st_mode    = d->flags & (S_IFMT | S_IRWXA),
	};

	return 0;
}

int lstat(const char *path, struct stat *buf) {
	return stat(path, buf);
}
