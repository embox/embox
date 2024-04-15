/**
 * @file
 * @brief Test if active running threads of the same priority
 * are swithed in a SCHED_RR manner.
 *
 * @date 06.06.18
 * @author Alex Kalmuk
 */

#include <errno.h>
#include <unistd.h>
#include <embox/test.h>
#include <util/err.h>
#include <kernel/thread.h>

#define DELAY  OPTION_GET(NUMBER, delay)

EMBOX_TEST_SUITE("Test for running threads switches");

static int thread_res[2];

static void delay_busy(int volatile delay) {
	while (delay--)
		;
}

static void *second_thread_hnd(void *arg) {
	int i;

	thread_yield();

	for (i = 0; i < 10; i++) {
		delay_busy(DELAY);
		thread_res[1] = i + 1;
	}

	test_assert(thread_res[0] > 0);
	
	return NULL;
}

static void *first_thread_hnd(void *arg) {
	struct thread *t;
	int i;

	t = thread_create(0, second_thread_hnd, &thread_res[1]);
	test_assert_zero(ptr2err(t));

	thread_yield();

	test_assert_zero(ptr2err(t));

	for (i = 0; i < 10; i++) {
		delay_busy(DELAY);
		thread_res[0] = i + 1;
	}

	test_assert(thread_res[1] > 0);

	test_assert_zero(thread_join(t, NULL));
	
	return NULL;
}

TEST_CASE("Test for two running thread switches") {
	struct thread *t;

	t = thread_create(0, first_thread_hnd, NULL);
	test_assert_zero(ptr2err(t));

	test_assert_zero(thread_join(t, NULL));
}
