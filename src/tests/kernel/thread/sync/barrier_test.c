/**
 * @file
 * @brief Tests barrier wait method.
 *
 * @date Sep 03, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>
#include <kernel/thread/sync/barrier.h>
#include <unistd.h>

static struct thread *low, *high;
static barrier_t b;

EMBOX_TEST_SUITE("Mutex test");

TEST_SETUP(setup);

TEST_CASE("General") {
	test_assert_zero(thread_resume(low));
	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(high, NULL));
	test_assert_emitted("abcde");
}

static void *low_run(void *arg) {
	test_emit('a');
	test_assert_zero(thread_resume(high));
	test_emit('c');
	barrier_wait(&b);
	test_emit('e');
	return NULL;
}

static void *high_run(void *arg) {
	test_emit('b');
	barrier_wait(&b);
	test_emit('d');
	return NULL;
}

static int setup(void) {
	thread_priority_t l = 10, h = 20;

	barrier_init(&b, 2);
	test_assert_zero(thread_create(&low, THREAD_FLAG_SUSPENDED, low_run, NULL));
	test_assert_zero(thread_create(&high, THREAD_FLAG_SUSPENDED, high_run, NULL));
	test_assert_zero(thread_set_priority(low, l));
	test_assert_zero(thread_set_priority(high, h));

	return 0;
}

