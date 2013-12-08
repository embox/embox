/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    27.11.2013
 */

#include <kernel/thread.h>
#include <kernel/sched.h>
#include <embox/test.h>

#include <kernel/sched/waitq.h>

EMBOX_TEST_SUITE("Test suite for basic waitq operations");

TEST_CASE("Sched_wake should leave thread's state consistent (as tested by "
		"__waitq_prepare)") {
	struct wait_link wait_link;
	struct waitq waitq;

	waitq_init(&waitq);

	__waitq_prepare(&waitq, &wait_link);

	sched_wake(thread_self());

	__waitq_cleanup();

	__waitq_prepare(&waitq, &wait_link);
	__waitq_cleanup();
}

TEST_CASE("Three-step event waiting should support event occured before wait") {
	struct wait_link wait_link;
	struct waitq waitq;
	int res;

	waitq_init(&waitq);

	__waitq_prepare(&waitq, &wait_link);

	waitq_notify_all(&waitq);

	res = __waitq_wait(10);

	test_assert_zero(res);

	__waitq_cleanup();

	__waitq_prepare(&waitq, &wait_link);
	__waitq_cleanup();
}

