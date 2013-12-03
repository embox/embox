/**
 * @file
 * @brief Multi-thread sleep
 *
 * @date 05.07.11
 * @author Ilia Vaprol
 * @author Fedor Burdun
 */

#include <errno.h>
#include <sys/types.h>

#include <kernel/sched.h>

int ksleep(useconds_t msec) {
	int wait_res;

	if (msec == 0) {
		sched_post_switch();
		return 0;
	}

	wait_res = SCHED_WAIT_TIMEOUT(0, msec);

	return wait_res;
}

