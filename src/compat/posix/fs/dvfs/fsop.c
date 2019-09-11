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

	if (lu.item)
		return SET_ERRNO(EEXIST);

	parent[0] = '\0';
	strncat(parent, pathname, sizeof(parent) - 1);
	if (parent[strlen(parent) - 1] == '/')
		parent[strlen(parent) - 1] = '\0';

	t = strrchr(parent, '/');
	if (t) {
		memset(t + 1, '\0', parent + DVFS_MAX_PATH_LEN - t);

		if ((res = dvfs_lookup(parent, &lu)))
			return SET_ERRNO(-res);

		lu.parent = lu.item;
		lu.item = NULL;
	} else {
		parent[0] = '\0';
		if ((res = dvfs_lookup(pathname, &lu))) {
			return SET_ERRNO(-res);
		}
	}

	res = dvfs_create_new(pathname + strlen(parent), &lu,
			       S_IFDIR | (mode & DVFS_DIR_VIRTUAL));
	if (res) {
		return SET_ERRNO(-res);
	}

	return 0;
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

/**
 * @brief Change length of regular file
 *
 * @param path Path of the regular file
 * @param length New length
 *
 * @return Negative error code or 0 if succeed
 *
 * @note Currently unsupported errors:
 *    EACCESS
 *    EFAULT
 *    EINTR
 *    EIO
 *    ENAMETOOLONG
 *    ENOTDIR
 *    EROFS
 **/
int truncate(const char *path, off_t length) {
	struct lookup lu;
	int err;

	assert(path);
	if (length < 0)
		return -EINVAL;

	if ((err = dvfs_lookup(path, &lu))) {
		return err;
	}

	if (FILE_TYPE(lu.item->flags, S_IFDIR))
		return -EISDIR;

	if (!FILE_TYPE(lu.item->flags, S_IFREG))
		return -EINVAL;

	assert(lu.item->d_inode);
	assert(lu.item->d_inode->i_ops);

	if (!lu.item->d_inode->i_ops->truncate)
		return -EPERM;

	return lu.item->d_inode->i_ops->truncate(lu.item->d_inode, length);
}

int flock(int fd, int operation) {
	return 0;
}

