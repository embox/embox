/**
 * @file
 * @brief
 *
 * @date 05.06.12
 * @author Anton Bondarev
 * @author Anton Kozlov
 */

#include <unistd.h>
#include <errno.h>

#include <kernel/task.h>
#include <kernel/task/idesc_table.h>
#include <kernel/task/idx.h>

#define IDESC_TABLE_USE

int dup(int flides) {
#ifndef IDESC_TABLE_USE
	int new_fd;

	if (!task_valid_binded_fd(flides)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if ((new_fd = task_self_idx_first_unbinded()) < 0) {
		SET_ERRNO(EMFILE);
		return -1;
	}

	return dup2(flides, new_fd);
#else
	struct idesc *idesc;
	struct idesc_table *it;
	int res;


	if (!idesc_index_valid(flides)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	it = idesc_table_get_table(task_self());
	assert(it);

	idesc = idesc_table_get_desc(it, flides);

	if (idesc == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	res = idesc_table_add(it, idesc, 0);

	if (res < 0) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
#endif
}

int dup2(int flides, int flides2) {
#ifndef IDESC_TABLE_USE
	int res;
	struct idx_desc *old_idx;

	if (!task_valid_binded_fd(flides)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (!task_valid_fd(flides2)) {
		SET_ERRNO(EMFILE);
		return -1;
	}

	if (flides == flides2) {
		return flides2;
	}

	old_idx = task_self_idx_get(flides);

	res = task_self_idx_set(flides2, task_idx_desc_alloc(old_idx->data));
	if (res != 0) {
		SET_ERRNO(-res);
		return -1;
	}

	return flides2;
#else
	struct idesc *idesc;
	struct idesc_table *it;
	int res;


	if (flides == flides2) {
		SET_ERRNO(EBADF);
		return -1;
	}
	if (!idesc_index_valid(flides2) || !idesc_index_valid(flides)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	it = idesc_table_get_table(task_self());
	assert(it);

	idesc = idesc_table_get_desc(it, flides);

	if (idesc == NULL) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (idesc_table_locked(it,flides2)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	res = idesc_table_lock(it, idesc, flides2, 0);

	if (res < 0) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;

#endif

}
