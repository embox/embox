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
	const char *t;
	size_t namelen;
	const char *dirname;
	int i;

	dvfs_lookup(pathname, &lu);

	/* TODO check lu->parent == parent that we want */

	if (lu.item)
		return SET_ERRNO(EEXIST);

	/* TODO rewrite this piece of code. The problem is optional trailing slash */
	namelen = strlen(pathname);
	dirname = t = pathname;
	for (i = 0; i < namelen; i++) {
		if (pathname[i] == '/') {
			dirname = t;
			t = &pathname[i];
		}
	}

	if (pathname[namelen - 1] != '/')
		dirname = t;

	if (dirname[0] == '/')
		dirname++;

	return dvfs_create_new(dirname, &lu, S_IFDIR);
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

