/**
 * @file
 * @brief TODO documentation for general_test.c -- Bulychev Anton
 *
 * @date Apr 2, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>

#include <errno.h>

#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/sched/wait_queue.h>

EMBOX_TEST_SUITE("Scheduler strategy general test");

TEST_CASE("sleep/wake") {
	struct wait_queue wq;

	wait_queue_init(&wq);
	test_assert_equal(wait_queue_wait(&wq, 10), -ETIMEDOUT);
}
