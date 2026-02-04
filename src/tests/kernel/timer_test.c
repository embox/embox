/**
 * @file
 *
 * @date 28.01.09
 * @author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#include <unistd.h>
#include <embox/test.h>
#include <kernel/time/sys_timer.h>
#include <kernel/time/time.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/sched/sync/mutex.h>

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

	if (sys_timer_set(&timer, SYS_TIMER_ONESHOT, TEST_TIMER_PERIOD, test_timer_handler,
			(void *) &tick_happened)) {
		test_fail("failed to install timer");
	}

	i = -1;

	while (i-- && !tick_happened) {
	}

	sys_timer_close(timer);

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

	if (sys_timer_set(&timer, SYS_TIMER_ONESHOT, 50, test_timer_handler1, (void *) &data)) {
		test_fail("failed to install timer");
	}

	if (sys_timer_set(&timer2, SYS_TIMER_ONESHOT, 100, test_timer_handler2, (void *) &data)) {
		test_fail("failed to install timer");
	}

	i = -1;

	while (i-- && !data.tick_happened1 && !data.tick_happened2) {
	}

	sys_timer_close(timer);
	sys_timer_close(timer2);

	test_assert(0 == data.error);
}

static void test_timer_handler_fail(sys_timer_t* tmr, void *param) {
	test_fail("should not occur");
}

TEST_CASE("Timer could be closed before it's occur") {
	struct sys_timer tmr;

	test_assert(0 == sys_timer_init_start_msec(&tmr, SYS_TIMER_ONESHOT, 50, test_timer_handler_fail, NULL));
	sys_timer_close(&tmr);
	usleep(100 * 1000);
}

TEST_CASE("Timer could be started many times") {
	struct sys_timer tmr;

	test_assert(0 == sys_timer_init(&tmr, SYS_TIMER_ONESHOT, test_timer_handler_fail, NULL));

	for (int try = 0; try < 10; ++try) {
		sys_timer_start(&tmr, ms2jiffies(50));
		usleep(40 * 1000);
	}

	sys_timer_close(&tmr);
}

struct timer_mutex {
	int counter;
	struct mutex mutex;
};

static void test_timer_handler_mutex(sys_timer_t *timer, void *param){
	struct timer_mutex *m;

	test_assert(param);

	m = param;

	if (mutex_trylock(&m->mutex)) {
		test_fail("Cannot lock mutex inside timer handler");
	}

	if (mutex_unlock(&m->mutex)) {
		test_fail("Cannot unlock mutex inside timer handler");
	}

	m->counter++;
}

TEST_CASE("setting mutex inside timer handler") {
	int i;
	sys_timer_t *timer;
	struct timer_mutex m;

	m.counter = 0;
	mutex_init(&m.mutex);

	if (sys_timer_set(	&timer,
			SYS_TIMER_ONESHOT,
			500,
			test_timer_handler_mutex,
			&m)) {
		test_fail("failed to install timer");
	}

	i = 10;

	while (i-- && m.counter == 0) {
		sleep(1);
	}

	sys_timer_close(timer);

	test_assert(m.counter > 0);
}
