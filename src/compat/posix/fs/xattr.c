/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.02.2013
 */

#include <errno.h>
#include <fs/vfs.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <fs/perm.h>
#include <security/security.h>

#include <sys/xattr.h>

int getxattr(const char *path, const char *name, char *value, size_t size) {
	int res;
	struct node *node;

	if (0 != (res = fs_perm_lookup(vfs_get_root(), path, NULL, &node))) {
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

int fgetxattr(int fd, const char *name, void *value, size_t size) {
	struct file_desc *file = (struct file_desc *)
		task_idx_desc_data(task_self_idx_get(fd));
	int res;

	if (!file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	assert(file->node);

	if (0 > (res = security_xattr_get(file->node, name, value, size))) {
		SET_ERRNO(EACCES);
		return -1;
	}

	if (0 > (res = kfile_xattr_get(file->node, name, value, size))) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
}

int setxattr(const char *path, const char *name, const char *value, size_t size,
	       	int flags) {
	int res;
	struct node *node;

	if (0 != (res = fs_perm_lookup(vfs_get_root(), path, NULL, &node))) {
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

int fsetxattr(int fd, const char *name, const char *value, size_t size, int flags) {
	struct file_desc *file = (struct file_desc *)
		task_idx_desc_data(task_self_idx_get(fd));
	int res;

	if (!file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	assert(file->node);

	if (0 > (res = security_xattr_set(file->node, name, value, size, flags))) {
		SET_ERRNO(EACCES);
		return -1;
	}


	if (0 > (res = kfile_xattr_set(file->node, name, value, size, flags))) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
}


int listxattr(const char *path, char *list, size_t size) {
	int res;
	struct node *node;

	if (0 != (res = fs_perm_lookup(vfs_get_root(), path, NULL, &node))) {
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

int flistxattr(int fd, char *list, size_t size) {
	struct file_desc *file = (struct file_desc *)
		task_idx_desc_data(task_self_idx_get(fd));
	int res;

	if (!file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	assert(file->node);

	if (0 > (res = security_xattr_list(file->node, list, size))) {
		SET_ERRNO(EACCES);
		return -1;
	}

	if (0 > (res = kfile_xattr_list(file->node, list, size))) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
}
