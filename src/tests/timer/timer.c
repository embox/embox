/**
 * @file
 *
 * @date 28.01.09
 * @author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#include <embox/test.h>
#include <kernel/timer.h>

#define TEST_TIMER_TICKS 10

EMBOX_TEST(run);

static void test_timer_handler(sys_timer_t* timer, void *param) {
	*(bool *)param = true;
}

static int run(void) {
	long i;
	sys_timer_t * timer;
	bool tick_happened;

	/* Timer value changing means ok */
	tick_happened = false;

	if (timer_set(&timer, TEST_TIMER_TICKS, test_timer_handler, &tick_happened)) {
		test_fail("failed to install timer");
	}
	for (i = 0; i < (1 << 30); i++) {
		if (tick_happened) {
			break;
		}
	}
	timer_close(timer);

	return tick_happened ? 0 : -1;
}
