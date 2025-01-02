/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 23.12.24
 */

#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#include <kernel/time/time.h>
#include <kernel/time/timer.h>
#include <plc/core.h>

#include "iec_types.h"

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
IEC_TIME __CURRENT_TIME;
IEC_BOOL __DEBUG;

static sys_timer_t *plc_timer;
static unsigned long tick_counter;
static bool plc_started;

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

bool plc_is_started(void) {
	return plc_started;
}

int plc_start(void) {
	uint32_t ticktime_ms;
	int err;

	if (plc_started) {
		return -1;
	}

	config_init__();

	tick_counter = 0;
	ticktime_ms = common_ticktime__ / NSEC_PER_MSEC;

	err = timer_set(&plc_timer, TIMER_PERIODIC, ticktime_ms, plc_handler, NULL);
	if (err) {
		return err;
	}

	plc_started = true;

	return 0;
}

int plc_stop(void) {
	if (!plc_started) {
		return -1;
	}

	timer_close(plc_timer);

	plc_started = false;

	return 0;
}
