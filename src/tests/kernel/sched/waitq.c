/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    27.11.2013
 */

#include <kernel/thread.h>
#include <embox/test.h>

#include <kernel/sched/waitq.h>
#include <kernel/thread/thread_sched_wait.h>

EMBOX_TEST_SUITE("Test suite for basic waitq operations");

TEST_CASE("Sched_wake should leave thread's state consistent (as tested by "
		"__waitq_prepare)") {
	struct waitq_link waitq_link;
	struct waitq waitq;

	waitq_init(&waitq);
	waitq_link_init(&waitq_link);

	waitq_wait_prepare(&waitq, &waitq_link);

	sched_wakeup(&thread_self()->schedee);

	waitq_wait_prepare(&waitq, &waitq_link);
	waitq_wait_cleanup(&waitq, &waitq_link);
}

TEST_CASE("Three-step event waiting should support event occured before wait") {
	struct waitq_link waitq_link;
	struct waitq waitq;
	int res;

	waitq_init(&waitq);
	waitq_link_init(&waitq_link);

	waitq_wait_prepare(&waitq, &waitq_link);

	waitq_wakeup_all(&waitq);

	res = sched_wait_timeout(10, NULL);

	test_assert_zero(res);

	waitq_wait_prepare(&waitq, &waitq_link);
	waitq_wait_cleanup(&waitq, &waitq_link);
}

