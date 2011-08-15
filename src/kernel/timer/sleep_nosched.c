/**
 * @file
 * @brief Single-thread sleep
 *
 * @date 09.02.09
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 */

#include <types.h>
#include <time.h>
#include <kernel/timer.h>
#include <hal/arch.h>

static void wake_up(sys_tmr_t *timer, void *param) {
	*(int *)param = 0;
}

/*system library function */
int usleep(useconds_t usec) {
	volatile int wait_flag; // for sleep func
	sys_tmr_t timer;
	wait_flag = 1;
	/* FIXME set_timer argument is tick (not usec) */
	if (init_timer(&timer, usec, &wake_up, (void *) &wait_flag)) {
		return 1;
	}
	while (wait_flag) {
		arch_idle();
	}
	close_timer(&timer);
	return 0;
}

int sleep(unsigned int seconds) {
	return usleep(seconds * 1000);
}
