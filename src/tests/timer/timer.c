/**
 * @file
 *
 * @date 28.01.09
 * @author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#include <embox/test.h>
#include <kernel/timer.h>

#define TEST_TIMER_ID    17
#define TEST_TIMER_TICKS 2

EMBOX_TEST(run);

volatile static bool tick_happened;

static void test_timer_handler(uint32_t id) {
	tick_happened = true;
}

static int run(void) {
	long i;
	int ret;

	/* Timer value changing means ok */
	tick_happened = false;

	ret = set_timer(TEST_TIMER_ID, TEST_TIMER_TICKS, test_timer_handler);
	if (ret == 0) {
		return -1;
	}
	for (i = 0; i < (1 << 30); i++) {
		if (tick_happened)
			break;
	}
	close_timer(TEST_TIMER_ID);

	return tick_happened ? 0 : -1;
}
