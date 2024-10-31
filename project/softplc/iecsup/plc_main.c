/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 20.10.24
 */

#include <time.h>
#include <unistd.h>

#include <kernel/thread/thread_sched_wait.h>
#include <kernel/time/time.h>
#include <kernel/time/timer.h>

#include "iec_std_lib.h"

/**
 * Functions and variables provied by generated C softPLC
 */
extern unsigned long long common_ticktime__;
extern unsigned long greatest_tick_count__;

extern void config_run__(int tick);
extern void config_init__(void);

/**
 * Functions and variables to export to generated C softPLC
 */
TIME __CURRENT_TIME;
BOOL __DEBUG;

static void plc_handler(sys_timer_t *timer, void *param) {
	unsigned int *tick;
	struct timespec ts;

	tick = ((unsigned int *)param);

	*tick += 1;
	if (greatest_tick_count__) {
		*tick %= greatest_tick_count__;
	}

	clock_gettime(CLOCK_REALTIME, &ts);

	__CURRENT_TIME.tv_sec = ts.tv_sec;
	__CURRENT_TIME.tv_nsec = ts.tv_nsec;

	config_run__(*tick);
}

int main(int argc, char const *argv[]) {
	volatile unsigned int tick;
	sys_timer_t *timer;
	uint32_t period;
	int err;

	config_init__();

	tick = 0;
	period = common_ticktime__ / NSEC_PER_MSEC;

	err = timer_set(&timer, TIMER_PERIODIC, period, plc_handler, (void *)&tick);

	if (!err) {
		err = SCHED_WAIT(0);
	}

	return err;
}
