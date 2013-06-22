/**
 * @file
 * @brief Multi-thread sleep
 *
 * @date 05.07.11
 * @author Ilia Vaprol
 * @author Fedor Burdun
 */

#include <errno.h>
#include <kernel/time/ktime.h>

#include <kernel/time/timer.h>
#include <kernel/sched.h>
#include <kernel/event.h>

int ksleep(useconds_t msec) {
	int res_sleep;
	struct event never_happen;

	if (msec == 0) {
		sched_post_switch();
		return 0;
	}

	event_init(&never_happen, "never_happen");

	res_sleep = EVENT_WAIT(&never_happen, 0, msec);

	return res_sleep == -ETIMEDOUT ? 0 : res_sleep;
}

