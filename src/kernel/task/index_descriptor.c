/**
 * @file
 *
 * @date Nov 29, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <fcntl.h>

#include <kernel/task.h>

#include <kernel/task/idesc_table.h>
#include <fs/idesc.h>

#include <fs/index_descriptor.h>

struct idesc *index_descriptor_get(int idx) {
	struct idesc_table *it;

	it = task_get_idesc_table(task_self());
	assert(it);

	return idesc_table_get(it, idx);
}

int index_descritor_cloexec_get(int fd) {
	struct idesc_table *it;
	int fd_flags = 0;

	it = task_get_idesc_table(task_self());
	assert(it);

	if (idesc_is_cloexeced(it->idesc_table[fd])) {
		fd_flags |= FD_CLOEXEC;
	}
	return fd_flags;
}

int index_descriptor_cloexec_set(int fd, int cloexec) {
	struct idesc_table *it;

	it = task_get_idesc_table(task_self());
	assert(it);

	if (cloexec | FD_CLOEXEC) {
		idesc_cloexec_set(it->idesc_table[fd]);
	} else {
		idesc_cloexec_clear(it->idesc_table[fd]);
	}
	return 0;
}

int index_descriptor_flags_get(int fd) {
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);

	assert(idesc);
	return idesc->idesc_flags;
}

int index_descriptor_flags_set(int fd, int flags) {
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);

	assert(idesc);
	idesc->idesc_flags = flags;

	return 0;
}

int index_descriptor_ioctl(int fd, int req, void *data) {
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);

	assert(idesc);
	assert(idesc->idesc_ops);

	if (!idesc->idesc_ops->ioctl) {
		return -ENOTSUP;
	}
	return idesc->idesc_ops->ioctl(idesc, req, data);

}

int index_descriptor_fcntl(int fd, int cmd, void *data) {
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);

	assert(idesc);
	assert(idesc->idesc_ops);
	if (!idesc->idesc_ops->ioctl) {
		return -ENOTSUP;
	}
	return idesc->idesc_ops->ioctl(idesc, cmd, data);
}

int index_descriptor_status(int fd, int pollmask) {
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);

	assert(idesc);
	assert(idesc->idesc_ops);
	if (!idesc->idesc_ops->status) {
		return -ENOTSUP;
	}
	return idesc->idesc_ops->status(idesc, pollmask);
}
