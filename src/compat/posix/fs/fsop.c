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
	return res;
}

int flock(int fd, int operation) {
	return kflock(fd, operation);
}

