/**
 * @file
 * @brief Tests semaphore enter/leave methods.
 *
 * @date Sep 03, 2012
 * @author Bulychev Anton
 */

#include <embox/test.h>
#include <kernel/thread/sync/semaphore.h>
#include <kernel/thread.h>
#include <err.h>

static struct thread *low, *mid, *high;
static struct sem s;

EMBOX_TEST_SUITE("Semaphore test");

TEST_SETUP(setup);

TEST_CASE("General") {
	test_assert_zero(thread_launch(low));
	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(mid, NULL));
	test_assert_zero(thread_join(high, NULL));
	test_assert_emitted("abcdefghijk");
}

static void *low_run(void *arg) {
	test_emit('a');
	semaphore_enter(&s);
	test_emit('b');
	test_assert_zero(thread_launch(mid));
	test_emit('j');
	semaphore_leave(&s);
	test_emit('k');
	return NULL;
}

static void *mid_run(void *arg) {
	test_emit('c');
	semaphore_enter(&s);
	test_emit('d');
	test_assert_zero(thread_launch(high));
	test_emit('f');
	semaphore_leave(&s);
	test_emit('i');
	return NULL;
}

static void *high_run(void *arg) {
	test_emit('e');
	semaphore_enter(&s);
	test_emit('g');
	semaphore_leave(&s);
	test_emit('h');
	return NULL;
}

static int setup(void) {
	sched_priority_t l = 200, m = 210, h = 220;

	semaphore_init(&s, 2);

	low = thread_create(THREAD_FLAG_SUSPENDED, low_run, NULL);
	test_assert_zero(err(low));

	mid = thread_create(THREAD_FLAG_SUSPENDED, mid_run, NULL);
	test_assert_zero(err(mid));

	high = thread_create(THREAD_FLAG_SUSPENDED, high_run, NULL);
	test_assert_zero(err(high));

	test_assert_zero(thread_set_priority(low, l));
	test_assert_zero(thread_set_priority(mid, m));
	test_assert_zero(thread_set_priority(high, h));

	return 0;
}
