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
	struct event_set e_set;;

	if (usec == 0) {
		sched_yield();
		return 0;
	}

	event_set_init(&e_set);
	event_set_add(&e_set, &never_happen);

	res_sleep = event_wait(never_happen.set, usec);

	event_set_clear(&e_set);

	return res_sleep == -ETIMEDOUT ? 0 : res_sleep;
}

int sleep(unsigned int seconds) {
	return usleep(seconds * 1000);
}

