/**
 * @file
 * @brief Tests barrier wait method.
 *
 * @date Sep 03, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>
#include <kernel/thread/sync/barrier.h>
#include <kernel/thread.h>

#include <util/err.h>

static struct thread *low, *high;
static barrier_t b;

EMBOX_TEST_SUITE("Barrier test");

TEST_SETUP(setup);

TEST_CASE("General") {
	test_assert_zero(thread_launch(low));
	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(high, NULL));
	test_assert_emitted("abcde");
}

static void *low_run(void *arg) {
	test_emit('a');
	test_assert_zero(thread_launch(high));
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
	int l = 200, h = 210;

	barrier_init(&b, 2);

	low = thread_create(THREAD_FLAG_SUSPENDED, low_run, NULL);
	test_assert_zero(ptr2err(low));

	high = thread_create(THREAD_FLAG_SUSPENDED, high_run, NULL);
	test_assert_zero(ptr2err(high));

	test_assert_zero(schedee_priority_set(&low->schedee, l));
	test_assert_zero(schedee_priority_set(&high->schedee, h));

	return 0;
}
