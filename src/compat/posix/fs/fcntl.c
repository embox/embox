/**
 * @file
 *
 * @date Nov 21, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>

#include <fcntl.h>

#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>
#include <kernel/task/idesc_table.h>

#include <fs/index_descriptor.h>

#include <fs/idesc.h>




static inline int dupfd(struct idesc *idesc, int idx) {
	struct idesc_table *it;
	int res;

	it = task_get_idesc_table(task_self());
	assert(it);

	if (idesc_table_locked(it, idx)) {
		assert(idx == 0); /* only for dup() */
		res = idesc_table_add(it, idesc, 0);
	} else {
		res = idesc_table_lock(it, idesc, idx, 0);
	}

	if (res < 0) {
		return SET_ERRNO(-res);
	}

	return res;
}


int fcntl(int fd, int cmd, ...) {
	struct idesc *idesc;
	void * data;
	va_list args;
	int dint;
	int res;

	if (!idesc_index_valid(fd)) {
		return SET_ERRNO(EBADF);
	}

	idesc = index_descriptor_get(fd);
	if (NULL == idesc) {
		return SET_ERRNO(EBADF);
	}

	/* Fcntl works in two steps:
	 * 1. Make general commands like F_SETFD, F_GETFD.
	 * 2. If fd has some internal fcntl(), it will be called.
	 *    Otherwise result of point 1 will be returned. */
	switch (cmd) {
	case F_DUPFD:
		va_start(args, cmd);
		dint = va_arg(args, int);
		va_end(args);
		return dupfd(idesc, dint);
	case F_GETFL:
		return idesc->idesc_flags;
	case F_SETFL:
		va_start(args, cmd);
		idesc->idesc_flags = va_arg(args, int);
		va_end(args);
		return 0;
	case F_GETFD: /* only for CLOEXEC flag */
		return index_descritor_cloexec_get(fd);
	case F_SETFD: /* only for CLOEXEC flag */
		va_start(args, cmd);
		dint = va_arg(args, int);
		va_end(args);
		return index_descriptor_cloexec_set(fd, dint);
/*	case F_GETPIPE_SZ:
	case F_SETPIPE_SZ:
		break;
*/

	default:
		va_start(args, cmd);
		data = va_arg(args, void*);
		va_end(args);
		res = index_descriptor_fcntl(fd, cmd, data);
		if (res < 0) {
			return SET_ERRNO(ENOTSUP);
		}
	}

	return 0;
}

