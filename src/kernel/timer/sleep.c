/**
 * @file
 * @brief Multi-thread sleep
 *
 * @date 05.07.11
 * @author Ilia Vaprol
 * @author Fedor Burdun
 */

#include <types.h>
#include <time.h>
#include <kernel/timer.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/event.h>

static void restore_thread(sys_timer_t *timer, void *param) {
	sched_wake((struct event *) param);
}

/* system library function */
int usleep(useconds_t usec) {
	struct event wait_event;
	sys_timer_t tmr; /* we allocate timer structure on the stack */

	event_init(&wait_event, NULL);

	sched_lock();

	if (timer_init(&tmr, usec, &restore_thread, &wait_event)) {
		return 1;
	}

	sched_sleep_locked(&wait_event);

	timer_close(&tmr);

	sched_unlock();
	return 0;
}

int sleep(unsigned int seconds) {
	return usleep(seconds * 1000);
}
