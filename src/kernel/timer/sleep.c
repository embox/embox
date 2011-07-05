/**
 * @file
 * @brief Multi-thread sleep
 *
 * @date 05.07.11
 * @author Ilia Vaprol
 */

#include <types.h>
#include <kernel/timer.h>
#include <kernel/thread/sched.h>

#define TIMER_FREQUENCY 1000

static void restore_thread(uint32_t id) {
	sched_wake(get_timer_event_by_id(id));
	close_timer(id);
}

/*system library function */
int usleep(useconds_t usec) {
	uint32_t id;

	sched_lock();

	if (!(id = get_free_timer_id()) || !set_timer(id, usec, &restore_thread)) {
		return 1;
	}
	sched_sleep_locked(get_timer_event_by_id(id));

	sched_unlock();

	return 0;
}

int sleep(uint32_t seconds) {
	return usleep(seconds * TIMER_FREQUENCY);
}
