/**
 * @file
 * @brief
 *
 * @date 11.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <sys/resource.h>
#include <unistd.h>

int nice(int incr) {
	int prior;

	errno = 0;
	prior = getpriority(PRIO_PROCESS, 0);
	if ((prior == -1) && (errno != 0)) {
		return -1;
	}

	return setpriority(PRIO_PROCESS, 0, prior + incr);
}
