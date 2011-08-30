/**
 * @file
 *
 * @date 28.01.09
 * @author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#include <embox/test.h>
#include <kernel/timer.h>

EMBOX_TEST_SUITE("basic timer tests");

#define TEST_TIMER_PERIOD      100 /*in ms*/

static void test_timer_handler(sys_timer_t* timer, void *param) {
	*(int *)param = 1;
}

TEST_CASE("testing timer_set function")  {
	long i;
	sys_timer_t * timer;
	volatile int tick_happened;

	/* Timer value changing means ok */
	tick_happened = 0;

	if (timer_set(&timer, TEST_TIMER_PERIOD, test_timer_handler,(void *) &tick_happened)) {
		test_fail("failed to install timer");
	}
	for (i = 0; i < (1 << 30); i++) {
		if (tick_happened) {
			break;
		}
	}
	timer_close(timer);

	test_assert_not_zero(tick_happened);
}
