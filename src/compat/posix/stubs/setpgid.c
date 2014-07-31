/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.05.2014
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int setpgid(pid_t pid, pid_t pgid) {
	fprintf(stderr, ">>>%s\n", __func__);
	return -ENOSYS;
}


