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
#include <fs/xattr.h>
#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_table.h>

#include <security/security.h>

#include <sys/xattr.h>

int getxattr(const char *path, const char *name, char *value, size_t size) {
	int res;
	struct path node;

	if (0 != (res = fs_perm_lookup(path, NULL, &node))) {
		return SET_ERRNO(-res);
	}

	if (0 > (res = security_xattr_get(node.node, name, value, size))) {
		return SET_ERRNO(EACCES);
	}

	if (0 > (res = kfile_xattr_get(node.node, name, value, size))) {
		return SET_ERRNO(-res);
	}

	return res;
}

int setxattr(const char *path, const char *name, const char *value, size_t size,
		int flags) {
	int res;
	struct path node;

	if (0 != (res = fs_perm_lookup(path, NULL, &node))) {
		return SET_ERRNO(-res);
	}

	if (0 > (res = security_xattr_set(node.node, name, value, size, flags))) {
		return SET_ERRNO(EACCES);
	}

	if (0 > (res = kfile_xattr_set(node.node, name, value, size, flags))) {
		return SET_ERRNO(-res);
	}

	return res;
}

int listxattr(const char *path, char *list, size_t size) {
	int res;
	struct path node;

	if (0 != (res = fs_perm_lookup(path, NULL, &node))) {
		return SET_ERRNO(-res);
	}

	if (0 > (res = security_xattr_list(node.node, list, size))) {
		return SET_ERRNO(EACCES);
	}

	if (0 > (res = kfile_xattr_list(node.node, list, size))) {
		return SET_ERRNO(-res);
	}

	return res;
}

int fsetxattr(int fd, const char *name, const char *value, size_t size,
		int flags) {
	struct idesc *idesc;
	int res;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))) {
		return SET_ERRNO(EBADF);
	}

	if (0 > (res = security_xattr_idesc_set(idesc, name, value, size, flags))) {
		return SET_ERRNO(EACCES);
	}

	if (0 > (res = idesc_setxattr(idesc, name, value, size, flags))) {
		return SET_ERRNO(-res);
	}

	return res;
}

int fgetxattr(int fd, const char *name, void *value, size_t size) {
	struct idesc *idesc;
	int res;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))) {
		return SET_ERRNO(EBADF);
	}

	if (0 > (res = security_xattr_idesc_get(idesc, name, value, size))) {
		return SET_ERRNO(EACCES);
	}

	if (0 > (res = idesc_getxattr(idesc, name, value, size))) {
		return SET_ERRNO(-res);
	}

	return res;
}

int flistxattr(int fd, char *list, size_t size) {
	struct idesc *idesc;
	int res;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))) {
		return SET_ERRNO(EBADF);
	}

	if (0 > (res = security_xattr_idesc_list(idesc, list, size))) {
		return SET_ERRNO(EACCES);
	}

	if (0 > (res = idesc_listxattr(idesc, list, size))) {
		return SET_ERRNO(-res);
	}

	return res;
}
