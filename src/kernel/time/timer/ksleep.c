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
#include <kernel/thread/thread_sched_wait.h>

int ksleep(useconds_t msec) {
	int wait_res;

	if (msec == 0) {
		sched_post_switch();
		return 0;
	}

	wait_res = SCHED_WAIT_TIMEOUT(0, msec);
	if (wait_res == -ETIMEDOUT) {
		return 0;
	}

	return wait_res;
}

