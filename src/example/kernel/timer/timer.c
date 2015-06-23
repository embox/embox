/**
 * @file
 *
 * @brief
 *
 * @date 16.08.2011
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <errno.h>
#include <framework/example/self.h>

#include <kernel/time/timer.h>

#define TEST_TIMER_TICKS 100 /* 100ms */

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

static void timer_handler(sys_timer_t* timer, void *param) {
	*(volatile int *) param += 1;
}

static inline bool wait_tick(int *tmr_cnt) {
	int i;
	for (i = 0; i < (1 << 30); i++) {
		if (*tmr_cnt > 10) {
			return true;
		}
	}
	return false;
}

static int timer_init_example(void) {
	int res;
	sys_timer_t timer;
	volatile int tick_cnt;

	/* this value will change during execution callback handler */
	tick_cnt = false;

	if (ENOERR
			!= (res = timer_init_start_msec(&timer, TIMER_ONESHOT, TEST_TIMER_TICKS,
					timer_handler, (void *) &tick_cnt))) {
		printf("failed to install timer");
		return res;
	}

	/* wait until timer handler will be called */
	wait_tick((int *) &tick_cnt);

	/* delete from queue */
	timer_close(&timer);

	return ENOERR;
}

static int timer_set_example(void) {
	sys_timer_t *timer;
	int res;
	int tick_cnt;

	/* this value will change during execution callback handler */
	tick_cnt = 0;

	if (ENOERR
			!= (res = timer_set(&timer, TIMER_PERIODIC, TEST_TIMER_TICKS, timer_handler,
					&tick_cnt))) {
		printf("failed to install timer");
		return res;
	}

	/* wait until timer handler will be called */
	wait_tick((int *) &tick_cnt);

	/* delete from queue */
	timer_close(timer);

	return ENOERR;
}

/**
 * Example's execution routing
 * It has been declare with macro EMBOX_EXAMPLE
 */
static int run(int argc, char **argv) {
	timer_init_example();
	timer_set_example();

	return ENOERR;
}
