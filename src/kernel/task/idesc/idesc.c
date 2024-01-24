/**
 * @file
 *
 * @date Oct 21, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#include <lib/libds/dlist.h>

#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <kernel/task/resource/idesc.h>

int idesc_init(struct idesc *idesc, const struct idesc_ops *ops, mode_t amode) {

	memset(idesc, 0, sizeof(struct idesc));

	idesc->idesc_flags = amode;

	idesc->idesc_ops = ops;
	idesc->idesc_xattrops = NULL;

	waitq_init(&idesc->idesc_waitq);

	return 0;
}

int idesc_check_mode(struct idesc *idesc, mode_t amode) {
	return (idesc->idesc_flags & O_ACCESS_MASK) == amode;
}

int idesc_close(struct idesc *idesc, int fd) {
	struct idesc_table *it;

	it = task_resource_idesc_table(task_self());
	assert(it);
	idesc_table_del(it, fd);

	return 0;
}

static int idesc_xattr_check(struct idesc *idesc) {
	if (!idesc) {
		return -EBADF;
	}

	if (!idesc->idesc_xattrops) {
		return -ENOTSUP;
	}

	assert(idesc->idesc_xattrops->getxattr);
	assert(idesc->idesc_xattrops->setxattr);
	assert(idesc->idesc_xattrops->listxattr);
	assert(idesc->idesc_xattrops->removexattr);

	return 0;
}

int idesc_getxattr(struct idesc *idesc, const char *name, void *value,
		size_t size) {
	int res;

	res = idesc_xattr_check(idesc);
	if (0 != res) {
		return res;
	}

	return idesc->idesc_xattrops->getxattr(idesc, name, value, size);
}

int idesc_setxattr(struct idesc *idesc, const char *name, const void *value,
		size_t size, int flags) {
	int res;

	res = idesc_xattr_check(idesc);
	if (0 != res) {
		return res;
	}

	return idesc->idesc_xattrops->setxattr(idesc, name, value, size, flags);
}

int idesc_listxattr(struct idesc *idesc, char *list, size_t size) {
	int res;

	res = idesc_xattr_check(idesc);
	if (0 != res) {
		return res;
	}

	return idesc->idesc_xattrops->listxattr(idesc, list, size);
}

int idesc_removexattr(struct idesc *idesc, const char *name) {
	int res;

	res = idesc_xattr_check(idesc);
	if (0 != res) {
		return res;
	}

	return idesc->idesc_xattrops->removexattr(idesc, name);
}

