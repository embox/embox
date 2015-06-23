/**
 * @file
 *
 * @date 28.01.09
 * @author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#include <unistd.h>
#include <embox/test.h>
#include <kernel/time/timer.h>
#include <kernel/time/time.h>

EMBOX_TEST_SUITE("basic timer tests");

#define TEST_TIMER_PERIOD      100 /* milliseconds */

static void test_timer_handler(sys_timer_t* timer, void *param) {
	*((int *) param) = 1;
}

TEST_CASE("testing timer_set function") {
	unsigned long i;
	sys_timer_t * timer;
	volatile int tick_happened;

	/* Timer value changing means ok */
	tick_happened = 0;

	if (timer_set(&timer, TIMER_ONESHOT, TEST_TIMER_PERIOD, test_timer_handler,
			(void *) &tick_happened)) {
		test_fail("failed to install timer");
	}

	i = -1;

	while (i-- && !tick_happened) {
	}

	timer_close(timer);

	test_assert(tick_happened);
}

struct test2_data {
	int tick_happened1;
	int tick_happened2;
	int error;
};

static void test_timer_handler1(sys_timer_t* timer, void *param) {
	struct test2_data *data = (struct test2_data *) param;

	if (data->tick_happened2 != 0) {
		data->error = 1;
	}

	data->tick_happened1 = 1;

}

static void test_timer_handler2(sys_timer_t* timer, void *param) {
	struct test2_data *data = (struct test2_data *) param;

	data->tick_happened2 = 1;
}

TEST_CASE("Testing 2 timer_set, one must occur earlier.") {
	unsigned long i;
	sys_timer_t * timer, * timer2;

	volatile struct test2_data data = {0, 0, 0};

	if (timer_set(&timer, TIMER_ONESHOT, 50, test_timer_handler1, (void *) &data)) {
		test_fail("failed to install timer");
	}

	if (timer_set(&timer2, TIMER_ONESHOT, 100, test_timer_handler2, (void *) &data)) {
		test_fail("failed to install timer");
	}

	i = -1;

	while (i-- && !data.tick_happened1 && !data.tick_happened2) {
	}

	timer_close(timer);
	timer_close(timer2);

	test_assert(0 == data.error);
}

static void test_timer_handler_fail(sys_timer_t* tmr, void *param) {
	test_fail("should not occur");
}

TEST_CASE("Timer could be closed before it's occur") {
	struct sys_timer tmr;

	test_assert(0 == timer_init_start_msec(&tmr, TIMER_ONESHOT, 50, test_timer_handler_fail, NULL));
	timer_close(&tmr);
	usleep(100 * 1000);
}

TEST_CASE("Timer could be started many times") {
	struct sys_timer tmr;

	test_assert(0 == timer_init(&tmr, TIMER_ONESHOT, test_timer_handler_fail, NULL));

	for (int try = 0; try < 10; ++try) {
		timer_start(&tmr, ms2jiffies(50));
		usleep(40 * 1000);
	}

	timer_close(&tmr);
}
