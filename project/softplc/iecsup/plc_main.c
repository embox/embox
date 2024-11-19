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
extern unsigned long long common_ticktime__; // ns
extern unsigned long greatest_tick_count__;  // tick

extern void config_init__(void);
extern void config_run__(unsigned long tick);

/**
 * Functions and variables to export to generated C softPLC
 */
TIME __CURRENT_TIME;
BOOL __DEBUG;

static unsigned long tick_counter;

static void plc_handler(sys_timer_t *timer, void *param) {
	struct timespec ts;

	tick_counter += 1;
	if (greatest_tick_count__) {
		tick_counter %= greatest_tick_count__;
	}

	clock_gettime(CLOCK_REALTIME, &ts);

	__CURRENT_TIME.tv_sec = ts.tv_sec;
	__CURRENT_TIME.tv_nsec = ts.tv_nsec;

	config_run__(tick_counter);
}

int main(int argc, char const *argv[]) {
	sys_timer_t *timer;
	uint32_t ticktime_ms;
	int err;

	config_init__();

	tick_counter = 0;
	ticktime_ms = common_ticktime__ / NSEC_PER_MSEC;

	err = timer_set(&timer, TIMER_PERIODIC, ticktime_ms, plc_handler, NULL);

	if (!err) {
		err = SCHED_WAIT(0);
	}

	return err;
}
