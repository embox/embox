/**
 * @file
 *
 * @brief
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <fs/dvfs.h>

int mkdir(const char *pathname, mode_t mode) {
	struct lookup lu = { };
	char *t;
	int res;

	char parent[DVFS_MAX_PATH_LEN];

	dvfs_lookup(pathname, &lu);

	if (lu.item) {
		dentry_ref_dec(lu.item);
		return SET_ERRNO(EEXIST);
	}

	parent[0] = '\0';
	strncat(parent, pathname, sizeof(parent) - 1);
	if (parent[strlen(parent) - 1] == '/')
		parent[strlen(parent) - 1] = '\0';

	t = strrchr(parent, '/');
	if (t) {
		memset(t + 1, '\0', parent + DVFS_MAX_PATH_LEN - t);

		if ((res = dvfs_lookup(parent, &lu))) {
			return SET_ERRNO(-res);
		}

		lu.parent = lu.item;
		lu.item = NULL;
	} else {
		parent[0] = '\0';
		if ((res = dvfs_lookup(pathname, &lu))) {
			return SET_ERRNO(-res);
		}
	}

	res = dvfs_create_new(pathname + strlen(parent), &lu,
			       S_IFDIR | (mode & VFS_DIR_VIRTUAL));
	dentry_ref_dec(lu.parent);
	dentry_ref_dec(lu.item);
	if (res) {
		return SET_ERRNO(-res);
	}

	return 0;
}

int remove(const char *pathname) {
	int ret = dvfs_remove(pathname);
	if (ret != 0) {
		return SET_ERRNO(-ret);
	}

	return 0;
}

int unlink(const char *pathname) {
	return 0;
}

int rmdir(const char *pathname) {
	return 0;
}

int flock(int fd, int operation) {
	return 0;
}

