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

int dup(int flides) {
	return fcntl(flides, F_DUPFD, 0);
}

int dup2(int flides, int flides2) {
	if (flides == flides2) {
		return SET_ERRNO(EBADF);
	}

	close(flides2);
	return fcntl(flides, F_DUPFD, flides2);
}
