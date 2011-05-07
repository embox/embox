/**
 * @file
 * @brief Tests thread suspend/resume methods.
 *
 * @date Apr 17, 2011
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <kernel/thread/api.h>

EMBOX_TEST_SUITE("Thread suspend/resume");

TEST_EMIT_BUFFER_DEF(buff1, 'f' - 'a' + 1);

static void *low_run(void *);
static void *high_run(void *);

TEST_CASE() {
	struct thread *low = NULL, *high = NULL;
	void *ret;

	test_emit_buffer_reset(&buff1);

	test_assert_zero(
			thread_create(&high, THREAD_FLAG_SUSPENDED, high_run, NULL));
	test_assert_not_null(high);
	test_assert_zero(thread_set_priority(high, THREAD_PRIORITY_HIGH));

	test_assert_zero(thread_create(&low, THREAD_FLAG_SUSPENDED, low_run, high));
	test_assert_not_null(low);
	test_assert_zero(thread_set_priority(low, THREAD_PRIORITY_LOW));

	test_emit(&buff1, 'a');

	test_assert_zero(thread_resume(low));
	test_assert_zero(thread_join(low, &ret));
	test_assert_null(ret);

	test_emit(&buff1, 'e');

	test_assert_zero(thread_resume(high));
	test_assert_zero(thread_join(high, &ret));
	test_assert_null(ret);

	test_assert_str_equal(test_emit_buffer_str(&buff1), "abcdef");
}

static void *low_run(void *arg) {
	struct thread *high = (struct thread *) arg;

	test_emit(&buff1, 'b');
	thread_resume(high);
	test_emit(&buff1, 'd');

	return NULL;
}

static void *high_run(void *arg) {
	test_emit(&buff1, 'c');
	thread_suspend(thread_self());
	test_emit(&buff1, 'f');

	return NULL;
}
