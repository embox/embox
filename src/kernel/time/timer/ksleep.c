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
#include <kernel/time/ktime.h>

#include <kernel/time/timer.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/event.h>

int ksleep(useconds_t usec) {
	int res_sleep;
	struct event never_happen;

	if (usec == 0) {
		sched_post_switch();
		return 0;
	}

	event_init(&never_happen, "never_happen");

	res_sleep = event_wait(&never_happen, usec);

	return res_sleep == -ETIMEDOUT ? 0 : res_sleep;
}

