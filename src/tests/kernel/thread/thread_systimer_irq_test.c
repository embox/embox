/**
 * @file
 * @brief Tests if System Timer's IRQ are handling when thread is running.
 *
 * @details There was a problme on STM32 platform:
 * when after context switch timers, interrupts become disabled
 * inside the next thread.
 *
 * @date 06.06.18
 * @author Alex Kalmuk
 */

#include <errno.h>
#include <unistd.h>
#include <embox/test.h>
#include <util/err.h>
#include <kernel/thread.h>
#include <hal/clock.h>

#define DELAY  1000000
#define ITERS  10

EMBOX_TEST_SUITE("test for systimer irq handling when thread is running");

static int thread_res[2];

static int delay_spinning(int volatile delay) {
	clock_t start = clock_sys_ticks();
	while (delay--)
		;
	return clock_sys_ticks() - start;
}

static void *thread_hnd(void *arg) {
	int val;
	int i = 0;

	for (i = 0; i < ITERS; i++) {
		val = delay_spinning(DELAY);
		if (!val) {
			break;
		}
	}

	*(int *) arg = val;
	
	return NULL;
}

TEST_CASE("System timer interrupts must not be disabled after thread_switch") {
	struct thread *t1, *t2;
	int val;

	/* Check first if clocks are updating during time */
	val = delay_spinning(DELAY);
	test_assert_not_equal(val, 0);

	t1 = thread_create(0, thread_hnd, &thread_res[0]);
	test_assert_zero(err(t1));
	t2 = thread_create(0, thread_hnd, &thread_res[1]);
	test_assert_zero(err(t2));

	test_assert_zero(thread_join(t1, NULL));
	test_assert_not_equal(thread_res[0], 0);
	test_assert_zero(thread_join(t2, NULL));
	test_assert_not_equal(thread_res[1], 0);
}
