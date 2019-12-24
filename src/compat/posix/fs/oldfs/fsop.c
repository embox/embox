/**
 * @file
 *
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>

#include <fs/kfsop.h>
#include <fs/vfs.h>
#include <fs/perm.h>
#include <fs/kfile.h>
#include <fs/inode.h>

extern mode_t umask_modify(mode_t newmode);

int mkdir(const char *pathname, mode_t mode) {
	int rc;

	rc = kmkdir(pathname, umask_modify(mode));

	return rc;
}

int remove(const char *pathname) {
	int rc;

	rc = kremove(pathname);

	return rc;
}

int unlink(const char *pathname) {
	int rc;

	rc = kunlink(pathname);

	return rc;
}

int rmdir(const char *pathname) {
	int rc;

	rc = krmdir(pathname);

	return rc;
}

int flock(int fd, int operation) {
	return kflock(fd, operation);
}
