/**
 * @file
 *
 * @date Oct 17, 2012
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <embox/test.h>
#include <kernel/time/sys_timer.h>

EMBOX_TEST_SUITE("oneshot timer tests");

#define TEST_TIMER_PERIOD      1 /* milliseconds */

static void test_timer_handler(sys_timer_t* timer, void *param) {
	*((int *) param) += 1;
}

TEST_CASE("testing sys_timer_set function") {
	int i;
	sys_timer_t * timer;
	volatile int tick_counter;

	/* Timer value changing means ok */
	tick_counter = 0;

	if (sys_timer_set(&timer, SYS_TIMER_ONESHOT, TEST_TIMER_PERIOD, test_timer_handler,
			(void *) &tick_counter)) {
		test_fail("failed to install timer");
	}

	i = 100000000;

	while (i--) {
	}

	sys_timer_close(timer);

	test_assert_equal(tick_counter, 1);
}
