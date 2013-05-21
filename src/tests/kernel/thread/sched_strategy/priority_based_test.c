/**
 * @file
 * @brief Tests priority_based scheduling algorithm.
 *
 * @date 19.03.2013
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <kernel/thread.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/wait_queue.h>

static struct thread *low, *high;
static struct wait_queue wq;

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
	wait_queue_notify(&wq);
	test_emit('e');
	return NULL;
}

static void *high_run(void *arg) {
	test_emit('b');
	wait_queue_wait(&wq, SCHED_TIMEOUT_INFINITE);
	test_emit('d');
	return NULL;
}

static int setup(void) {
	thread_priority_t l = 200, h = 210;

	wait_queue_init(&wq);
	test_assert_zero(thread_create(&low, THREAD_FLAG_SUSPENDED, low_run, NULL));
	test_assert_zero(thread_create(&high, THREAD_FLAG_SUSPENDED, high_run, NULL));
	test_assert_zero(thread_set_priority(low, l));
	test_assert_zero(thread_set_priority(high, h));
	return 0;
}
