/**
 * @file
 * @brief Multi-thread sleep
 *
 * @date 05.07.11
 * @author Ilia Vaprol
 * @author Fedor Burdun
 */

#include <errno.h>
#include <types.h>
#include <time.h>

#include <kernel/time/timer.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/event.h>

/* system library function */
int usleep(useconds_t usec) {
	int res_sleep;
	struct event never_happen;

	if (usec == 0) {
		sched_yield();
		return 0;
	}

	event_init(&never_happen, NULL);

	res_sleep = sched_sleep(&never_happen, usec);

	return res_sleep == SCHED_TIMEOUT_HAPPENED ? 0 : res_sleep;
}

int sleep(unsigned int seconds) {
	return usleep(seconds * 1000);
}

