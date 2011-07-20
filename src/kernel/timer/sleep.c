/**
 * @file
 * @brief Multi-thread sleep
 *
 * @date 05.07.11
 * @author Ilia Vaprol
 */

#include <types.h>
#include <time.h>
#include <kernel/timer.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/event.h>

#define USE_INIT_TIMER 1

static void restore_thread(sys_tmr_t *timer, void *param) {
	sched_wake((struct event *) param);
	close_timer(&timer);
}

/*system library function */
int usleep(useconds_t usec) {
	struct event wait_event;
#if USE_INIT_TIMER
	sys_tmr_t tmr;
	sys_tmr_t *ptmr = &tmr;
#endif

	event_init(&wait_event, NULL);

	sched_lock();

#if USE_INIT_TIMER
	if (init_timer(&ptmr, usec, &restore_thread, &wait_event)) {
		return 1;
	}
#else
	if (set_timer(NULL, usec, &restore_thread, &wait_event)) {
		return 1;
	}
#endif
	sched_sleep_locked(&wait_event);

	sched_unlock();
	return 0;
}

int sleep(unsigned int seconds) {
	return usleep(seconds * 1000);
}
