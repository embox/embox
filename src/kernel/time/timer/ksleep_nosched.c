/**
 * @file
 * @brief Single-thread sleep
 *
 * @date 09.02.09
 * @author Andrey Baboshin
 * @author Ilia Vaprol
 */

#include <sys/types.h>

#include <hal/cpu_idle.h>
#include <kernel/time/ktime.h>
#include <kernel/time/timer.h>

static void wake_up(sys_timer_t *timer, void *param) {
	*(int *)param = 0;
}

int ksleep(useconds_t msec) {
	volatile int wait_flag; // for sleep func
	sys_timer_t timer;

	if (0 == msec) {
		return 0;
	}

	wait_flag = 1;

	if (sys_timer_init_start_msec(&timer, SYS_TIMER_ONESHOT, msec, &wake_up,
	        (void *)&wait_flag)) {
		return 1;
	}
	while (wait_flag) {
		arch_cpu_idle();
	}
	sys_timer_close(&timer);
	return 0;
}
