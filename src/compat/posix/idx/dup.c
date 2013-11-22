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
#include <fcntl.h>

#include <kernel/task.h>
#include <kernel/task/idesc_table.h>
#include <kernel/task/idx.h>

int dup(int flides) {
	int res;

	if (!idesc_index_valid(flides)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	res = fcntl(flides, F_DUPFD, 0);

	return res;
}

int dup2(int flides, int flides2) {
	int res;

	if (flides == flides2) {
		SET_ERRNO(EBADF);
		return -1;
	}
	if (!idesc_index_valid(flides2) || !idesc_index_valid(flides)) {
		return SET_ERRNO(EBADF);
	}

	close(flides2);
	res = fcntl(flides, F_DUPFD, flides2);

	return res;


}
