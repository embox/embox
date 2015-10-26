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
	struct lookup lu;
	char *t;

	char parent[DVFS_MAX_PATH_LEN];

	dvfs_lookup(pathname, &lu);

	if (lu.item)
		return SET_ERRNO(EEXIST);

	strncpy(parent, pathname, sizeof(parent));
	if (parent[strlen(parent) - 1] == '/')
		parent[strlen(parent) - 1] = '\0';

	t = strrchr(parent, '/');
	if (t) {
		memset(t + 1, '\0', parent + DVFS_MAX_PATH_LEN - t);

		dvfs_lookup(parent, &lu);
		if (!lu.item)
			return SET_ERRNO(ENOENT);

		lu.parent = lu.item;
		lu.item = NULL;
	} else {
		parent[0] = '\0';
		dvfs_lookup(pathname, &lu);
	}

	return dvfs_create_new(pathname + strlen(parent),
	                       &lu,
			       S_IFDIR | (mode & DVFS_DIR_VIRTUAL));
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

int truncate(const char *path, off_t length) {
	return 0;
}

int flock(int fd, int operation) {
	return 0;
}

