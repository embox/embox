/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.02.2013
 */

#include <errno.h>
#include <fs/vfs.h>

#include <fs/perm.h>
#include <security/security.h>
#include <fs/index_descriptor.h>

#include <sys/xattr.h>

int getxattr(const char *path, const char *name, char *value, size_t size) {
	int res;
	struct node *node;

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), path, NULL, &node))) {
		SET_ERRNO(-res);
		return -1;
	}

	if (0 > (res = security_xattr_get(node, name, value, size))) {
		SET_ERRNO(EACCES);
		return -1;
	}

	if (0 > (res = kfile_xattr_get(node, name, value, size))) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
}

int setxattr(const char *path, const char *name, const char *value, size_t size,
	       	int flags) {
	int res;
	struct node *node;

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), path, NULL, &node))) {
		SET_ERRNO(-res);
		return -1;
	}

	if (0 > (res = security_xattr_set(node, name, value, size, flags))) {
		SET_ERRNO(EACCES);
		return -1;
	}

	if (0 > (res = kfile_xattr_set(node, name, value, size, flags))) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
}

int listxattr(const char *path, char *list, size_t size) {
	int res;
	struct node *node;

	if (0 != (res = fs_perm_lookup(vfs_get_leaf(), path, NULL, &node))) {
		SET_ERRNO(-res);
		return -1;
	}

	if (0 > (res = security_xattr_list(node, list, size))) {
		SET_ERRNO(EACCES);
		return -1;
	}

	if (0 > (res = kfile_xattr_list(node, list, size))) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
}

int fsetxattr(int fd, const char *name, const char *value, size_t size, int flags) {
	struct idesc *idesc;
	int res;

	idesc = index_descriptor_get(fd);

	if (!idesc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (0 > (res = security_xattr_idesc_set(idesc, name, value, size, flags))) {
		SET_ERRNO(EACCES);
		return -1;
	}

	if (0 > (res = idesc_setxattr(idesc, name, value, size, flags))) {
		return SET_ERRNO(-res);
	}

	return res;
}

int fgetxattr(int fd, const char *name, void *value, size_t size) {
	struct idesc *idesc;
	int res;

	idesc = index_descriptor_get(fd);

	if (!idesc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (0 > (res = security_xattr_idesc_get(idesc, name, value, size))) {
		SET_ERRNO(EACCES);
		return -1;
	}

	if (0 > (res = idesc_getxattr(idesc, name, value, size))) {
		return SET_ERRNO(-res);
	}

	return res;
}

int flistxattr(int fd, char *list, size_t size) {
	struct idesc *idesc;
	int res;

	idesc = index_descriptor_get(fd);

	if (!idesc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (0 > (res = security_xattr_idesc_list(idesc, list, size))) {
		SET_ERRNO(EACCES);
		return -1;
	}

	if (0 > (res = idesc_listxattr(idesc, list, size))) {
		return SET_ERRNO(-res);
	}

	return res;
}
