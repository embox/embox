/**
 * @file
 * @brief TODO documentation for core_test.c -- Alina Kramar
 *
 * @date 01.05.2011
 * @author Alina Kramar
 */

#include <embox/test.h>

#include <kernel/thread/api.h>
#include <kernel/thread/sync/mutex.h>

EMBOX_TEST_SUITE("Priority inheritance for mutex");
TEST_EMIT_BUFFER_DEF(buff, 22);

static void *low_run(void *);
static void *mid_run(void *);
static void *high_run(void *);

TEST_CASE("without inheritance") {
	struct thread *low = NULL, *mid = NULL, *high = NULL;
	struct mutex mutex;
	thread_priority_t l = 10, m = 20, h = 30;

	mutex_init(&mutex);

	test_emit_buffer_reset(&buff);

	test_assert_zero(
			thread_create(&low, THREAD_FLAG_SUSPENDED, low_run, (void *) &mutex));
	test_assert_not_null(low);
	test_assert_zero(thread_set_priority(low, l));
	thread_resume(low);

	test_assert_zero(
			thread_create(&mid, THREAD_FLAG_SUSPENDED, mid_run, NULL));
	test_assert_not_null(mid);
	test_assert_zero(thread_set_priority(mid, m));
	thread_resume(mid);

	test_assert_zero(
			thread_create(&high, THREAD_FLAG_SUSPENDED, high_run, (void *) &mutex));
	test_assert_not_null(high);
	test_assert_zero(thread_set_priority(high, h));
	thread_resume(high);

	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(mid, NULL));
	test_assert_zero(thread_join(high, NULL));

//	test_assert_str_equal(test_emit_buffer_str(&buff), "abceeecddba");
	TRACE("%s", test_emit_buffer_str(&buff));
}

static void *low_run(void *arg) {
	struct mutex *m = (struct mutex *) arg;

	test_emit(&buff, 'a');

	mutex_lock(m);

	for (int i = 0; i < 10; i++) {
		test_emit(&buff, 'c');
	}

	mutex_unlock(m);

	test_emit(&buff, 'a');

	return NULL;
}

static void *mid_run(void *arg) {
	test_emit(&buff, 'e');
	test_emit(&buff, 'e');
	test_emit(&buff, 'e');

	return NULL;
}

static void *high_run(void *arg) {
	struct mutex *m = (struct mutex *) arg;

	test_emit(&buff, 'b');

	mutex_lock(m);

	test_emit(&buff, 'd');
	test_emit(&buff, 'd');

	mutex_unlock(m);

	test_emit(&buff, 'b');

	return NULL;
}
