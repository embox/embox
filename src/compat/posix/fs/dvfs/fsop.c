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
#include <fs/kfsop.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/perm.h>
#include <fs/kfile.h>
#include <sys/types.h>
#include <stdio.h>

#include "getumask.h"

int creat(const char *pathname, mode_t mode) {
	int rc;

	rc = open(pathname, O_CREAT | O_WRONLY | O_TRUNC, mode);
	DPRINTF(("creat(%s, %d ...) = %d\n", pathname, mode, rc));
	return rc;
}

int mkdir(const char *pathname, mode_t mode) {
	int rc;

	rc = kmkdir(pathname, umask_modify(mode));
	DPRINTF(("mkdir(%s, %d ...) = %d\n", pathname, mode, rc));
	return rc;
}

int remove(const char *pathname) {
	int rc;

	rc = kremove(pathname);
	DPRINTF(("remove(%s) = %d\n", pathname, rc));
	return rc;
}

int unlink(const char *pathname) {
	int rc;

	rc = kunlink(pathname);
	DPRINTF(("unlink(%s) = %d\n", pathname, rc));
	return rc;
}

int rmdir(const char *pathname) {
	int rc;

	rc = krmdir(pathname);
	DPRINTF(("rmdir(%s) = %d\n", pathname, rc));
	return rc;
}

int lstat(const char *path, struct stat *buf) {
	int rc;

	rc = klstat(path, buf);
	DPRINTF(("lstat(%s) = %d\n", path, rc));
	return rc;
}

int stat(const char *path, struct stat *buf) {
	int rc;

	rc = lstat(path, buf);
	DPRINTF(("stat(%s) = %d\n", path, rc));
	return rc;
}

int truncate(const char *path, off_t length) {
	struct path node;
	int res;

	if (0 == (res = fs_perm_lookup(path, NULL, &node))) {
		errno = -res;
		res = -1;
		goto end;
	}

	res = ktruncate(node.node, length);
	end:
	DPRINTF(("truncate(%s, %d ...) = %d\n", path, length, res));
	return res;
}

int flock(int fd, int operation) {
	return kflock(fd, operation);
}

