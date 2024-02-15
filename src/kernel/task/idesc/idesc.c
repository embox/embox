/**
 * @file
 *
 * @date Oct 21, 2013
 * @author: Anton Bondarev
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

#include <framework/mod/options.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_table.h>
#include <lib/libds/dlist.h>
#include <mem/misc/pool.h>

#define IDESC_POOL_SIZE OPTION_GET(NUMBER, idesc_pool_size)

POOL_DEF(idesc_pool, struct idesc, IDESC_POOL_SIZE);

struct idesc *idesc_alloc(void) {
	return pool_alloc(&idesc_pool);
}

void idesc_free(struct idesc *idesc) {
	if (pool_belong(&idesc_pool, idesc)) {
		pool_free(&idesc_pool, idesc);
	}
}

int idesc_open(struct idesc *idesc, void *source) {
	if (!idesc->idesc_ops || !idesc->idesc_ops->open) {
		return -ENOTSUP;
	}

	return idesc->idesc_ops->open(idesc, source);
}

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
	idesc_free(idesc);

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
