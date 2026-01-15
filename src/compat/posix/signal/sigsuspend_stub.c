/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.05.2014
 */

#include <errno.h>
#include <stdio.h>
#include <signal.h>

int sigsuspend(const sigset_t *sigmask) {
	fprintf(stderr, ">>>%s\n", __func__);
	return -ENOSYS;
}



