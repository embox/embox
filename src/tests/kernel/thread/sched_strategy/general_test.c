/**
 * @file
 * @brief TODO documentation for general_test.c -- Bulychev Anton
 *
 * @date Apr 2, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>

#include <errno.h>

#include <kernel/thread/sched.h>
#include <kernel/thread.h>

EMBOX_TEST_SUITE("Scheduler strategy general test");

TEST_CASE("sleep/wake") {
	struct sleepq sq;

	sleepq_init(&sq);
	test_assert_equal(sched_sleep_ms(&sq, 10), -ETIMEDOUT);
}
