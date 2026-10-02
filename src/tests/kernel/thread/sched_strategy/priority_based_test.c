/**
 * @file
 * @brief Tests priority_based scheduling algorithm.
 *
 * @date 19.03.2013
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <kernel/thread.h>
#include <util/err.h>
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>
#include <kernel/thread/thread_sched_wait.h>

static struct thread *low, *high;
static struct waitq wq;

EMBOX_TEST_SUITE("Priority_based scheduling algorithm.");

TEST_SETUP(setup);

TEST_CASE("Sleep/wake") {
	test_assert_zero(thread_launch(low));
	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(high, NULL));
	test_assert_emitted("abcde");
}

static void *low_run(void *arg) {
	test_emit('a');
	test_assert_zero(thread_launch(high));
	test_emit('c');
	waitq_wakeup_all(&wq);
	test_emit('e');
	return NULL;
}

static void *high_run(void *arg) {
	struct waitq_link *wql = &thread_self()->schedee.waitq_link;

	waitq_link_init(wql);

	test_emit('b');

	waitq_wait_prepare(&wq, wql);
	sched_wait();
	waitq_wait_cleanup(&wq, wql);

	test_emit('d');

	return NULL;
}

static int setup(void) {
	int l = 200, h = 210;

	waitq_init(&wq);

	low = thread_create(THREAD_FLAG_SUSPENDED, low_run, NULL);
	test_assert_zero(ptr2err(low));

	high = thread_create(THREAD_FLAG_SUSPENDED, high_run, NULL);
	test_assert_zero(ptr2err(high));

	test_assert_zero(schedee_priority_set(&low->schedee, l));
	test_assert_zero(schedee_priority_set(&high->schedee, h));
	return 0;
}
